#include "libretroloader.h"

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QString>
#include <QStringBuilder>

#include "SDL.h"
#include "SDL_gamecontroller.h"

void LibretroLoader::commandIn( Command command, QVariant data, qint64 timeStamp ) {
    // Command is not relayed to children automatically

    switch( command ) {
        case Command::Load: {
            // Make sure we only connect on load
            if( !connectedToCore ) {
                connect( &libretroCore, &LibretroCore::dataOut, this, &Node::dataOut );
                connect( &libretroCore, &LibretroCore::commandOut, this, &Node::commandOut );
                connectedToCore = true;
            }

            qCDebug( phxCore ) << command;
            libretroCore.state = State::Loading;
            emit commandOut( Command::Load, QVariant(), nodeCurrentTime() );

            // Set paths (QFileInfo gives you convenience functions, for example to extract just the directory from a file path)
            libretroCore.coreFileInfo.setFile( libretroCore.source[ "core" ] );
            libretroCore.gameFileInfo.setFile( libretroCore.source[ "game" ] );
            libretroCore.systemPathInfo.setFile( libretroCore.source[ "systemPath" ] );
            libretroCore.savePathInfo.setFile( libretroCore.source[ "savePath" ] );

            libretroCore.coreFile.setFileName( libretroCore.coreFileInfo.absoluteFilePath() );
            libretroCore.gameFile.setFileName( libretroCore.gameFileInfo.absoluteFilePath() );

            libretroCore.contentPath.setPath( libretroCore.gameFileInfo.absolutePath() );
            libretroCore.systemPath.setPath( libretroCore.systemPathInfo.absolutePath() );
            libretroCore.savePath.setPath( libretroCore.savePathInfo.absolutePath() );

            // Convert to C-style ASCII strings (needed by the API)
            libretroCore.corePathByteArray = libretroCore.coreFileInfo.absolutePath().toLocal8Bit();
            libretroCore.gameFileByteArray = libretroCore.gameFileInfo.absoluteFilePath().toLocal8Bit();
            libretroCore.gamePathByteArray = libretroCore.gameFileInfo.absolutePath().toLocal8Bit();
            libretroCore.systemPathByteArray = libretroCore.systemPathInfo.absolutePath().toLocal8Bit();
            libretroCore.savePathByteArray = libretroCore.savePathInfo.absolutePath().toLocal8Bit();
            libretroCore.corePathCString = libretroCore.corePathByteArray.constData();
            libretroCore.gameFileCString = libretroCore.gameFileByteArray.constData();
            libretroCore.gamePathCString = libretroCore.gamePathByteArray.constData();
            libretroCore.systemPathCString = libretroCore.systemPathByteArray.constData();
            libretroCore.savePathCString = libretroCore.savePathByteArray.constData();

            qDebug() << "";
            qCDebug( phxCore ) << "Now loading:";
            qCDebug( phxCore ) << "Core        :" << libretroCore.source[ "core" ];
            qCDebug( phxCore ) << "Game        :" << libretroCore.source[ "game" ];
            qCDebug( phxCore ) << "System path :" << libretroCore.source[ "systemPath" ];
            qCDebug( phxCore ) << "Save path   :" << libretroCore.source[ "savePath" ];
            qDebug() << "";

            // Set defaults that'll get overwritten as the core loads if necessary
            {
                // Pixel format is set to QImage::Format_RGB16 by default by the struct ProducerFormat constructor
                // However, for Libretro the default is RGB1555 aka QImage::Format_RGB555
                libretroCore.videoFormat.videoPixelFormat = QImage::Format_RGB555;
            }

            // Load core
            {
                qCDebug( phxCore ) << "Loading core:" << libretroCore.coreFileInfo.absoluteFilePath();

                libretroCore.coreFile.load();

                // Resolve symbols
                resolved_sym( retro_set_environment );
                resolved_sym( retro_set_video_refresh );
                resolved_sym( retro_set_audio_sample );
                resolved_sym( retro_set_audio_sample_batch );
                resolved_sym( retro_set_input_poll );
                resolved_sym( retro_set_input_state );
                resolved_sym( retro_init );
                resolved_sym( retro_deinit );
                resolved_sym( retro_api_version );
                resolved_sym( retro_get_system_info );
                resolved_sym( retro_get_system_av_info );
                resolved_sym( retro_set_controller_port_device );
                resolved_sym( retro_reset );
                resolved_sym( retro_run );
                resolved_sym( retro_serialize );
                resolved_sym( retro_serialize_size );
                resolved_sym( retro_unserialize );
                resolved_sym( retro_cheat_reset );
                resolved_sym( retro_cheat_set );
                resolved_sym( retro_load_game );
                resolved_sym( retro_load_game_special );
                resolved_sym( retro_unload_game );
                resolved_sym( retro_get_region );
                resolved_sym( retro_get_memory_data );
                resolved_sym( retro_get_memory_size );

                qDebug() << "Core loaded:" << libretroCore.coreFile.isLoaded();
                qDebug() << "retro_api_version =" << reinterpret_cast<void *>( libretroCore.symbols.retro_api_version );

                // Set callbacks
                libretroCore.symbols.retro_set_environment( LibretroCoreEnvironmentCallback );
                libretroCore.symbols.retro_set_audio_sample( LibretroCoreAudioSampleCallback );
                libretroCore.symbols.retro_set_audio_sample_batch( LibretroCoreAudioSampleBatchCallback );
                libretroCore.symbols.retro_set_input_poll( LibretroCoreInputPollCallback );
                libretroCore.symbols.retro_set_input_state( LibretroCoreInputStateCallback );
                libretroCore.symbols.retro_set_video_refresh( LibretroCoreVideoRefreshCallback );

                // Init the core
                libretroCore.symbols.retro_init();

                // Get some info about the game
                libretroCore.symbols.retro_get_system_info( libretroCore.systemInfo );


                qDebug() << "";
            }

            // Flush stderr, some cores may still write to it despite having RETRO_LOG
            fflush( stderr );

            // Load game
            {
                qCDebug( phxCore ) << "Loading game:" << libretroCore.gameFileInfo.absoluteFilePath();

                // Argument struct for symbols.retro_load_game()
                retro_game_info gameInfo;

                // Full path needed, simply pass the game's file path to the core
                if( libretroCore.systemInfo->need_fullpath ) {
                    qCDebug( phxCore ) << "Passing file path to core...";
                    gameInfo.path = libretroCore.gameFileCString;
                    gameInfo.data = nullptr;
                    gameInfo.size = 0;
                    gameInfo.meta = "";
                }

                // Full path not needed, read the file to memory and pass that to the core
                else {
                    qCDebug( phxCore ) << "Copying game contents to memory...";
                    libretroCore.gameFile.open( QIODevice::ReadOnly );

                    // Read into memory
                    libretroCore.gameData = libretroCore.gameFile.readAll();

                    libretroCore.gameFile.close();

                    gameInfo.path = nullptr;
                    gameInfo.data = libretroCore.gameData.constData();
                    gameInfo.size = libretroCore.gameFile.size();
                    gameInfo.meta = "";
                }

                libretroCore.symbols.retro_load_game( &gameInfo );

                qDebug() << "";
            }

            // Flush stderr, some cores may still write to it despite having RETRO_LOG
            fflush( stderr );

            // Load save data
            LibretroCoreLoadSaveData();

            // Get audio/video timing and send to consumers, allocate buffer pool
            {
                // Get info from the core
                retro_system_av_info *avInfo = new retro_system_av_info();
                libretroCore.symbols.retro_get_system_av_info( avInfo );
                LibretroCoreGrowBufferPool( avInfo );
                qCDebug( phxCore ).nospace() << "coreFPS: " << avInfo->timing.fps;
                emit commandOut( Command::SetCoreFPS, ( qreal )( avInfo->timing.fps ), nodeCurrentTime() );

                // Create the FBO which contains the texture 3d cores will draw to
                if( libretroCore.videoFormat.videoMode == HARDWARERENDER ) {
                    libretroCore.context->makeCurrent( libretroCore.surface );

                    // If the core has made available its max width/height at this stage, recreate the FBO with those settings
                    // Otherwise, use a sensible default size, the core will probably set the proper size in the first frame
                    if( avInfo->geometry.max_width != 0 && avInfo->geometry.max_height != 0 ) {
                        if( libretroCore.fbo ) {
                            delete libretroCore.fbo;
                        }

                        libretroCore.fbo = new QOpenGLFramebufferObject( avInfo->geometry.base_width, avInfo->geometry.base_height, QOpenGLFramebufferObject::CombinedDepthStencil );

                        // Clear the newly created FBO
                        libretroCore.fbo->bind();
                        libretroCore.context->functions()->glClear( GL_COLOR_BUFFER_BIT );
                    } else {
                        if( libretroCore.fbo ) {
                            delete libretroCore.fbo;
                        }

                        libretroCore.fbo = new QOpenGLFramebufferObject( 640, 480, QOpenGLFramebufferObject::CombinedDepthStencil );

                        // Clear the newly created FBO
                        libretroCore.fbo->bind();
                        libretroCore.context->functions()->glClear( GL_COLOR_BUFFER_BIT );

                        avInfo->geometry.max_width = 640;
                        avInfo->geometry.max_height = 480;
                        avInfo->geometry.base_width = 640;
                        avInfo->geometry.base_height = 480;
                    }

                    // Tell any video output children about this texture
                    emit commandOut( Command::SetOpenGLTexture, libretroCore.fbo->texture(), nodeCurrentTime() );

                    libretroCore.symbols.retro_hw_context_reset();
                }

                libretroCore.getAVInfo( avInfo );
                delete avInfo;
            }

            // Set all variables to their defaults, mark all variables as dirty
            {
                for( const auto &key : libretroCore.variables.keys() ) {
                    LibretroVariable &variable = libretroCore.variables[ key ];

                    if( !variable.choices().size() ) {
                        continue;
                    }

                    // Assume the defualt choice to be the first option offered
                    QByteArray defaultChoice = variable.choices()[ 0 ];

                    if( defaultChoice.isEmpty() ) {
                        continue;
                    }

                    // Assign
                    variable.setValue( defaultChoice );

                    QVariant var;
                    var.setValue( variable );
                    emit commandOut( Command::SetLibretroVariable, var, nodeCurrentTime() );
                }

                libretroCore.variablesAreDirty = true;
            }

            // Disconnect LibretroCore from the rest of the pipeline
            disconnect( &libretroCore, &LibretroCore::dataOut, this, &Node::dataOut );
            disconnect( &libretroCore, &LibretroCore::commandOut, this, &Node::commandOut );
            connectedToCore = false;

            // Flush stderr, some cores may still write to it despite having RETRO_LOG
            fflush( stderr );

            libretroCore.pausable = true;
            emit commandOut( Command::SetPausable, true, nodeCurrentTime() );

            libretroCore.state = State::Paused;
            emit commandOut( Command::Pause, QVariant(), nodeCurrentTime() );

            break;
        }

        case Command::SetSource: {
            qCDebug( phxCore ) << command;
            emit commandOut( command, data, timeStamp );

            QMap<QString, QVariant> map = data.toMap();
            QStringMap stringMap;

            for( QString key : map.keys() ) {
                stringMap[ key ] = map[ key ].toString();
            }

            libretroCore.source = stringMap;
            break;
        }

        case Command::SetSurface: {
            emit commandOut( command, data, timeStamp );
            libretroCore.surface = data.value<QOffscreenSurface *>();
            break;
        }

        case Command::SetOpenGLContext: {
            emit commandOut( command, data, timeStamp );
            libretroCore.context = data.value<QOpenGLContext *>();
            break;
        }

        default: {
            emit commandOut( command, data, timeStamp );
            break;
        }
    }
}
