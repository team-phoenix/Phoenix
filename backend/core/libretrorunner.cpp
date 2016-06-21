#include "libretrorunner.h"
#include "mousestate.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QString>
#include <QStringBuilder>
#include <QThread>
#include <QMutexLocker>

#include "SDL.h"
#include "SDL_gamecontroller.h"
#include "SDL_haptic.h"

void LibretroRunner::commandIn( Command command, QVariant data, qint64 timeStamp ) {
    // Command is not relayed to children automatically

    switch( command ) {
        case Command::Play: {
            // Make sure we're only connecting LibretroCore to this node if it's a command that only shows up when
            // emulation is active (in other words, never during loading)
            if( !connectedToCore ) {
                connectedToCore = true;
                qDebug() << "LibretroRunner will now emit signals from LibretroCore";
                connect( &libretroCore, &LibretroCore::dataOut, this, &LibretroRunner::dataOut );
                connect( &libretroCore, &LibretroCore::commandOut, this, &LibretroRunner::commandOut );
            }

            qCDebug( phxCore ) << command;
            libretroCore.state = State::Playing;
            emit commandOut( Command::Play, QVariant(), nodeCurrentTime() );
            break;
        }

        case Command::Pause: {
            if( !connectedToCore ) {
                connectedToCore = true;
                qDebug() << "LibretroRunner will now emit signals from LibretroCore";
                connect( &libretroCore, &LibretroCore::dataOut, this, &LibretroRunner::dataOut );
                connect( &libretroCore, &LibretroCore::commandOut, this, &LibretroRunner::commandOut );
            }

            qCDebug( phxCore ) << command;
            libretroCore.state = State::Paused;
            emit commandOut( Command::Pause, QVariant(), nodeCurrentTime() );
            break;
        }

        case Command::Stop: {
            if( !connectedToCore ) {
                connectedToCore = true;
                qDebug() << "LibretroRunner will now emit signals from LibretroCore";
                connect( &libretroCore, &LibretroCore::dataOut, this, &LibretroRunner::dataOut );
                connect( &libretroCore, &LibretroCore::commandOut, this, &LibretroRunner::commandOut );
            }

            qCDebug( phxCore ) << command;
            libretroCore.state = State::Unloading;
            emit commandOut( Command::Unload, QVariant(), nodeCurrentTime() );

            // Write SRAM

            qCInfo( phxCore ) << "=======Saving game...=======";
            LibretroCoreStoreSaveData();
            qCInfo( phxCore ) << "============================";

            // Unload core
            {
                // symbols.retro_api_version is reasonably expected to be defined if the core is loaded
                if( libretroCore.symbols.retro_api_version ) {
                    libretroCore.symbols.retro_unload_game();
                    libretroCore.symbols.retro_deinit();
                    libretroCore.symbols.clear();
                    libretroCore.coreFile.unload();
                    qCDebug( phxCore ) << "Unloaded core successfully";
                } else {
                    qCCritical( phxCore ) << "stop() called on an unloaded core!";
                }
            }

            // Unload game (if we've read its contents into a buffer)
            {
                libretroCore.gameData.clear();
            }

            // Disconnect LibretroCore from the rest of the pipeline
            disconnect( &libretroCore, &LibretroCore::dataOut, this, &LibretroRunner::dataOut );
            disconnect( &libretroCore, &LibretroCore::commandOut, this, &LibretroRunner::commandOut );
            connectedToCore = false;

            // Delete the FBO
            {
                if( libretroCore.fbo ) {
                    delete libretroCore.fbo;
                }

                libretroCore.fbo = nullptr;
            }

            // Reset video mode to 2D (will be set to 3D if the next core asks for it)
            libretroCore.videoFormat.videoMode = SOFTWARERENDER;

            libretroCore.state = State::Stopped;
            emit commandOut( Command::Stop, QVariant(), nodeCurrentTime() );

            break;
        }

        case Command::Heartbeat: {
            // Drop any heartbeats from too far in the past
            if( nodeCurrentTime() - timeStamp > 50 ) {
                return;
            }

            emit commandOut( command, data, timeStamp );

            if( libretroCore.state == State::Playing ) {
                // If in 3D mode, lock the mutex before emulating then activate our context and FBO
                // This is because we're not sure exactly when the core will render to the texture. So, we'll just lock the
                // mutex for the *entire* frame to be safe and not just from the start of the frame until the video callback
                // In 2D mode it's simpler: We know that the data will come in a buffer which we can quickly copy within
                // the video callback.
                if( libretroCore.videoFormat.videoMode == HARDWARERENDER ) {
                    libretroCore.videoMutex.lock();
                    //qDebug() << "LibretroRunner lock";
                    libretroCore.context->makeCurrent( libretroCore.surface );
                    libretroCore.fbo->bind();
                }

                // Invoke libretro core
                libretroCore.symbols.retro_run();

                // Update rumble state
                // TODO: Apply per-controller
                for( GamepadState &gamepad : libretroCore.gamepads ) {
                    if( gamepad.instanceID == -1 || !gamepad.haptic ) {
                        //qDebug() << gamepad.instanceID << ( !gamepad.haptic ) << ( gamepad.hapticID < 0 );
                        continue;
                    }

                    else if( libretroCore.fallbackRumbleCurrentStrength[ gamepad.instanceID ] != gamepad.fallbackRumbleRequestedStrength ) {
                        //qDebug() << "from" << core.fallbackRumbleCurrentStrength[ gamepad.instanceID ] << "to" << gamepad.fallbackRumbleRequestedStrength;

                        libretroCore.fallbackRumbleCurrentStrength[ gamepad.instanceID ] = gamepad.fallbackRumbleRequestedStrength;

                        SDL_HapticRumbleStop( gamepad.haptic );

                        if( SDL_HapticRumblePlay( gamepad.haptic, libretroCore.fallbackRumbleCurrentStrength[ gamepad.instanceID ], SDL_HAPTIC_INFINITY ) != 0 ) {
                            qWarning() << gamepad.friendlyName << SDL_GetError();

                            qWarning().nospace() << gamepad.friendlyName << ": SDL_HapticRumblePlay( "
                                                 << gamepad.haptic << ", "
                                                 << libretroCore.fallbackRumbleCurrentStrength
                                                 << ", SDL_HAPTIC_INFINITY ) != 0, rumble not available";
                            qWarning() << "SDL:" << SDL_GetError();
                        }

                        // Reset the requested strength
                        // Implicitly reset by incoming Gamepads overwriting the value set by us with the default of 0.0
                        // gamepad.fallbackRumbleRequestedStrength = 0.0;
                    }
                }

                if( libretroCore.videoFormat.videoMode == HARDWARERENDER ) {
                    libretroCore.context->makeCurrent( libretroCore.surface );
                    libretroCore.context->functions()->glFlush();
                    libretroCore.context->doneCurrent();
                    //qDebug() << "LibretroRunner unlock";
                    libretroCore.videoMutex.unlock();
                }

                // Flush stderr, some cores may still write to it despite having RETRO_LOG
                fflush( stderr );
            }

            break;
        }

        case Command::SetWindowGeometry: {
            emit commandOut( command, data, timeStamp );
            libretroCore.windowGeometry = data.toRect();
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::SetAspectRatioMode: {
            libretroCore.aspectMode = data.toInt();
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::SetLibretroVariable: {
            LibretroVariable var = data.value<LibretroVariable>();
            libretroCore.variables.insert( var.key(), var );
            libretroCore.variablesAreDirty = true;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::AddController: {
            if( !connectedToCore ) {
                connectedToCore = true;
                qDebug() << "LibretroRunner will now emit signals from LibretroCore";
                connect( &libretroCore, &LibretroCore::dataOut, this, &LibretroRunner::dataOut );
                connect( &libretroCore, &LibretroCore::commandOut, this, &LibretroRunner::commandOut );
            }

            GamepadState gamepad = data.value<GamepadState>();
            int instanceID = gamepad.instanceID;
            libretroCore.fallbackRumbleCurrentStrength[ instanceID ] = 0.0;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::RemoveController: {
            if( !connectedToCore ) {
                connectedToCore = true;
                qDebug() << "LibretroRunner will now emit signals from LibretroCore";
                connect( &libretroCore, &LibretroCore::dataOut, this, &LibretroRunner::dataOut );
                connect( &libretroCore, &LibretroCore::commandOut, this, &LibretroRunner::commandOut );
            }

            GamepadState gamepad = data.value<GamepadState>();
            int instanceID = gamepad.instanceID;
            libretroCore.gamepads.remove( instanceID );
            emit commandOut( command, data, timeStamp );
            break;
        }

        default: {
            emit commandOut( command, data, timeStamp );
            break;
        }
    }
}

void LibretroRunner::dataIn( DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) {
    emit dataOut( type, mutex, data, bytes, timeStamp );

    switch( type ) {
        // Make a copy of the data into our own gamepad list
        case DataType::Input: {
            mutex->lock();
            GamepadState gamepad = *static_cast<GamepadState *>( data );
            mutex->unlock();
            int instanceID = gamepad.instanceID;
            libretroCore.gamepads[ instanceID ] = gamepad;
            break;
        }

        // Make a copy of the incoming data and store it
        case DataType::MouseInput: {
            mutex->lock();
            libretroCore.mouse = *static_cast<MouseState *>( data );
            mutex->unlock();
            break;
        }

        default:
            break;
    }
}
