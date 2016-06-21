#include "sdlmanager.h"

#include <QByteArray>
#include <QDateTime>
#include <QFile>

#include <memory>
#include <cstring>

#include "logging.h"

SDLManager::SDLManager( Node *parent ) : Node( parent ) {
    // Load the built-in mapping file
    QFile gameControllerDBFile( ":/input/gamecontrollerdb.txt" );
    bool status = gameControllerDBFile.open( QIODevice::ReadOnly );
    Q_ASSERT( status );
    QByteArray mappingData = gameControllerDBFile.readAll();
    SDL_SetHint( SDL_HINT_GAMECONTROLLERCONFIG, mappingData.constData() );
    gameControllerDBFile.close();

    // Init SDL
    if( SDL_Init( SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC ) < 0 ) {
        qFatal( "Fatal: Unable to initialize SDL2: %s", SDL_GetError() );
    }

    // Allow game controller and joystick event states to be included in SDL_PollEvent
    SDL_GameControllerEventState( SDL_ENABLE );
    SDL_JoystickEventState( SDL_ENABLE );
}

void SDLManager::commandIn( Node::Command command, QVariant data, qint64 timeStamp ) {
    switch( command ) {
        case Command::Heartbeat: {
            // Check input and connect/disconnect events, update accordingly
            SDL_Event sdlEvent;

            while( SDL_PollEvent( &sdlEvent ) ) {
                switch( sdlEvent.type ) {
                    // Grab all the info we can about the controller, add this info to an entry in the gamepads hash table
                    case SDL_JOYDEVICEADDED: {
                        int joystickID = sdlEvent.jdevice.which;

                        SDL_Joystick *joystickHandle = SDL_JoystickOpen( joystickID );
                        int instanceID = SDL_JoystickInstanceID( joystickHandle );

                        GamepadState &gamepad = gamepads[ instanceID ];
                        gamepad.joystickID = joystickID;
                        gamepad.instanceID = instanceID;
                        gamepad.joystickHandle = joystickHandle;
                        gamepad.GUID = SDL_JoystickGetGUID( joystickHandle );

                        // Stringify the GUID
                        {
                            QByteArray GUIDString( 33, '\0' );
                            SDL_JoystickGetGUIDString( gamepad.GUID, GUIDString.data(), 33 );
                            gamepad.GUIDString = GUIDString;
                        }
                        const char *friendlyName = SDL_JoystickName( joystickHandle );

                        if( friendlyName ) {
                            gamepad.friendlyName = QString::fromUtf8( friendlyName );
                        }

                        // Sanity check on the number of available axes, buttons and joysticks
                        {
                            // TODO: Support more than 16 axes?
                            if( ( gamepad.joystickNumAxes = SDL_JoystickNumAxes( joystickHandle ) ) > 16 ) {
                                qWarning() << "Ignoring controller with more than 16 axes, GUID ="
                                           << QByteArray( reinterpret_cast<const char *>( gamepad.GUID.data ), 16 );
                                SDL_JoystickClose( joystickHandle );
                                break;
                            }

                            // TODO: Support more than 16 hats?
                            if( ( gamepad.joystickNumHats = SDL_JoystickNumHats( joystickHandle ) ) > 16 ) {
                                qWarning() << "Ignoring controller with more than 16 hats, GUID ="
                                           << QByteArray( reinterpret_cast<const char *>( gamepad.GUID.data ), 16 );
                                SDL_JoystickClose( joystickHandle );
                                break;
                            }

                            // TODO: Support more than 256 buttons?
                            if( ( gamepad.joystickNumButtons = SDL_JoystickNumButtons( joystickHandle ) ) > 256 ) {
                                qWarning() << "Ignoring controller with more than 256 buttons, GUID ="
                                           << QByteArray( reinterpret_cast<const char *>( gamepad.GUID.data ), 16 );
                                SDL_JoystickClose( joystickHandle );
                                break;
                            }
                        }

                        // Print some info about the controller
                        {
                            qDebug().noquote() << "Added controller:" << gamepad.friendlyName
                                               << "joystickID:" << joystickID << "instanceID:"
                                               << instanceID << "Number of gamepads (SDL):" << SDL_NumJoysticks()
                                               << "Number of gamepads (Phoenix):" << gamepads.size();

                            if( !SDL_IsGameController( joystickID ) ) {
                                qInfo() << "Device has no mapping yet";
                            }
                        }

                        // Set up rumble support
                        {
                            SDL_Haptic *haptic = SDL_HapticOpenFromJoystick( joystickHandle );

                            if( !haptic ) {
                                qWarning() << "SDL_HapticOpenFromJoystick(" << joystickHandle << ") = nullptr, rumble not available";
                                qWarning() << "SDL:" << SDL_GetError();
                            }

                            if( haptic && ( SDL_HapticRumbleSupported( haptic ) == SDL_FALSE ) ) {
                                qWarning() << "SDL_HapticRumbleSupported(" << haptic << ") = SDL_FALSE, rumble not available";
                                SDL_HapticClose( haptic );
                                haptic = nullptr;
                            }

                            if( haptic && ( SDL_HapticRumbleInit( haptic ) != 0 ) ) {
                                qWarning() << "SDL_HapticRumbleInit(" << haptic << ") != 0, rumble not available";
                                qWarning() << "SDL:" << SDL_GetError();
                                SDL_HapticClose( haptic );
                                haptic = nullptr;
                            }

                            if( haptic && ( SDL_HapticRumblePlay( haptic, 0.0, SDL_HAPTIC_INFINITY ) != 0 ) ) {
                                qWarning().nospace() << "SDL_HapticRumblePlay( " << haptic << ", 0.0, SDL_HAPTIC_INFINITY ) != 0, rumble not available";
                                qWarning() << "SDL:" << SDL_GetError();
                                SDL_HapticClose( haptic );
                                haptic = nullptr;
                            }

                            if( haptic ) {
                                qDebug() << "Rumble initialized successfully, haptic =" << haptic;
                            }

                            // If we had failed at any step in initialization, this will be nullptr
                            gamepad.haptic = haptic;
                        }

                        // Set up the joystick as a game controller if a mapping is available
                        {
                            // If this is a game controller, reopen as one and grab some more info
                            SDL_GameController *gamecontrollerHandle = nullptr;

                            if( SDL_IsGameController( joystickID ) ) {
                                gamecontrollerHandle = SDL_GameControllerOpen( joystickID );
                                char *mappingString = SDL_GameControllerMapping( gamecontrollerHandle );
                                gamepad.mappingString = QString::fromUtf8( mappingString );
                                SDL_free( mappingString );
                                const char *friendlyName = SDL_GameControllerName( gamecontrollerHandle );

                                if( friendlyName ) {
                                    gamepad.friendlyName = QString::fromUtf8( friendlyName );
                                }

                                gamepad.gamecontrollerHandle = gamecontrollerHandle;
                            }
                        }

                        // Inform the consumers a new controller was just added
                        emit commandOut( Command::AddController, QVariant::fromValue( gamepad ), nodeCurrentTime() );

                        break;
                    }

                    // Remove the removed gamepad's entry from the gamepads hash table
                    case SDL_JOYDEVICEREMOVED: {
                        int instanceID = sdlEvent.jdevice.which;
                        GamepadState &gamepad = gamepads[ instanceID ];

                        // Inform the consumers this controller was removed so it can be deleted from their lists or otherwise marked as unplugged
                        emit commandOut( Command::RemoveController, QVariant::fromValue( gamepad ), nodeCurrentTime() );

                        // Remove it here, too
                        gamepads.remove( instanceID );

                        qCDebug( phxInput ) << "A controller has been removed, number of gamepads (Phoenix):" << gamepads.size();
                        break;
                    }

                    // Update our button state
                    case SDL_CONTROLLERBUTTONDOWN:
                    case SDL_CONTROLLERBUTTONUP: {
                        int instanceID = sdlEvent.cbutton.which;
                        Uint8 buttonID = sdlEvent.cbutton.button;
                        Uint8 state = sdlEvent.cbutton.state;
                        gamepads[ instanceID ].button[ buttonID ] = state;
                        break;
                    }

                    case SDL_CONTROLLERAXISMOTION: {
                        int instanceID = sdlEvent.caxis.which;
                        Uint8 axis = sdlEvent.caxis.axis;
                        Sint16 value = sdlEvent.caxis.value;
                        gamepads[ instanceID ].axis[ axis ] = value;
                        break;
                    }

                    case SDL_JOYAXISMOTION: {
                        int instanceID = sdlEvent.jaxis.which;
                        Uint8 axis = sdlEvent.jaxis.axis;
                        Sint16 value = sdlEvent.jaxis.value;
                        gamepads[ instanceID ].joystickAxis[ axis ] = value;
                        //qDebug() << instanceID << "axis motion, axis:" << axis << "value:" << value;
                        break;
                    }

                    case SDL_JOYBUTTONDOWN:
                    case SDL_JOYBUTTONUP: {
                        int instanceID = sdlEvent.jbutton.which;
                        Uint8 buttonID = sdlEvent.jbutton.button;
                        Uint8 state = sdlEvent.jbutton.state;
                        gamepads[ instanceID ].joystickButton[ buttonID ] = state;
                        //qDebug() << "button event," << "instanceID:" << instanceID << "buttonID:" << buttonID << "state:" << state;
                        break;
                    }

                    case SDL_JOYHATMOTION: {
                        int instanceID = sdlEvent.jhat.which;
                        Uint8 hat = sdlEvent.jhat.hat;
                        Uint8 value = sdlEvent.jhat.value;
                        gamepads[ instanceID ].joystickHat[ hat ] = value;
                        //qDebug() << instanceID << "hat motion, hat:" << hat << "value:" << value;
                        break;
                    }

                    default: {
                        break;
                    }
                }
            }

            // Emit an update for all connected controllers
            for( GamepadState gamepad : gamepads ) {
                // Copy current gamepad into buffer
                mutex.lock();
                gamepadBuffer[ gamepadBufferIndex ] = gamepad;
                mutex.unlock();

                // Send buffer on its way
                emit dataOut( DataType::Input, &mutex, &gamepadBuffer[ gamepadBufferIndex ], 0, nodeCurrentTime() );

                // Increment the index
                gamepadBufferIndex = ( gamepadBufferIndex + 1 ) % 100;
            }

            // Inject heartbeat into child nodes' event queues *after* input data
            emit commandOut( command, data, timeStamp );

            break;
        }

        case Command::SetUserDataPath: {
            userDataPath = data.toString();

            // Create the file if it doesn't exist yet
            QFile gameControllerDBFile( userDataPath + "/gamecontrollerdb.txt" );

            if( !gameControllerDBFile.exists() ) {
                gameControllerDBFile.open( QIODevice::ReadWrite );
                QTextStream stream( &gameControllerDBFile );
                stream << "# Insert your custom definitions here" << endl;
            }

            gameControllerDBFile.close();

            // Load remappings from user data file
            int ret = SDL_GameControllerAddMappingsFromFile( ( userDataPath + "/gamecontrollerdb.txt" ).toUtf8().constData() );

            if( ret >= 0 ) {
                qInfo() << "Added" << ret << "custom mappings from file" << userDataPath + "/gamecontrollerdb.txt";
                qInfo() << "Note: SDL2 may have also read overwrites that don't add to the above count";
            } else {
                qWarning() << "Unable to read mappings from file!" << userDataPath + "/gamecontrollerdb.txt" << SDL_GetError();
            }

            emit commandOut( command, data, timeStamp );
            break;
        }

        default: {
            emit commandOut( command, data, timeStamp );
            break;
        }
    }
}
