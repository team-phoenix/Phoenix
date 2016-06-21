#include "remapper.h"
#include "remappermodel.h"

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QFlags>
#include <QIODevice>
#include <QKeySequence>
#include <QTextStream>
#include <QVector2D>
#include <QtMath>

Remapper::Remapper() {
    keyboardGamepad.instanceID = -1;
}

// Public slots

void Remapper::commandIn( Command command, QVariant data, qint64 timeStamp ) {
    switch( command ) {
        case Command::Stop:
        case Command::Load:
        case Command::Pause:
        case Command::Unload:
        case Command::Reset: {
            emit commandOut( command, data, timeStamp );
            playing = false;
            break;
        }

        case Command::Play: {
            emit commandOut( command, data, timeStamp );
            playing = true;
            break;
        }

        case Command::HandleGlobalPipelineReady: {
            emit commandOut( command, data, timeStamp );
            emit controllerAdded( "", "Keyboard", 0, 0, 0 );

            // TODO: Read keyboard setting from disk too
            dpadToAnalogKeyboard = true;

            // Give the model friendly names for all the keys set
            for( int physicalKey : keyboardKeyToSDLButton.keys() ) {
                int virtualButton = keyboardKeyToSDLButton[ physicalKey ];

                // This prints garbage as-is, so change it to something that doesn't
                // Confirmed on Windows but looks okay in OS X?
                // https://bugreports.qt.io/browse/QTBUG-40030
                if( physicalKey == Qt::Key_Shift ) {
                    physicalKey = Qt::ShiftModifier;
                }

                if( physicalKey == Qt::Key_Control ) {
                    physicalKey = Qt::ControlModifier;
                }

                if( physicalKey == Qt::Key_Meta ) {
                    physicalKey = Qt::MetaModifier;
                }

                if( physicalKey == Qt::Key_Alt ) {
                    physicalKey = Qt::AltModifier;
                }

                if( physicalKey == Qt::Key_AltGr ) {
                    physicalKey = Qt::AltModifier;
                }

                QString keyString = QKeySequence( physicalKey ).toString( QKeySequence::NativeText );

                // Strip out trailing + in the corrected keys
                if( keyString.length() > 1 ) {
                    keyString = keyString.section( '+', 0, 0 );
                }

                emit setMapping( "",  keyString, gameControllerIDToMappingString( virtualButton ) );
            }

            break;
        }

        case Command::SetUserDataPath: {
            userDataPath = data.toString();
            qDebug() << "Using path" << userDataPath;
            break;
        }

        case Command::Heartbeat: {
            emit commandOut( command, data, timeStamp );

            // Tell the model to clear its stored states
            emit heartbeat();

            // Send out per-GUID OR'd states to RemapperModel then clear stored pressed states
            for( QString GUID : pressed.keys() ) {
                emit buttonUpdate( GUID, pressed[ GUID ] );
                pressed[ GUID ] = false;
            }

            // Do the same for the keyboard
            emit buttonUpdate( "", keyboardKeyPressed );
            keyboardKeyPressed = false;

            // If in remap mode, make sure the GUID in question still exists, exit remap mode if not
            if( remapMode && !GUIDCount.contains( remapModeGUID ) ) {
                qCWarning( phxInput ) << "No controllers with GUID" << remapModeGUID << "remaining, exiting remap mode!";
                remapMode = false;
                emit remappingEnded();
            }

            break;
        }

        case Command::AddController: {
            emit commandOut( command, data, timeStamp );
            GamepadState gamepad = data.value<GamepadState>();
            int instanceID = gamepad.instanceID;
            QString GUID( QByteArray( reinterpret_cast<const char *>( gamepad.GUID.data ), 16 ).toHex() );

            // Add to map if it hasn't been encountered yet
            // Otherwise, just increment the count
            if( !GUIDCount.contains( GUID ) ) {
                GUIDCount[ GUID ] = 1;
                emit controllerAdded( GUID, gamepad.friendlyName, gamepad.joystickNumButtons, gamepad.joystickNumHats, gamepad.joystickNumAxes );
            } else {
                GUIDCount[ GUID ]++;
            }

            // Initialize mappings with invalid values
            for( int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++ ) {
                Key key = Key( BUTTON, i );
                Val val = Val( INVALID, VHat( -1, -1 ) );
                gameControllerToJoystick[ GUID ][ key ] = val;
                emit setMapping( GUID, keyToMappingString( key ), valToFriendlyString( val ) );
            }

            // Grab gamepad handle
            gameControllerHandles[ instanceID ] = gamepad.gamecontrollerHandle;

            for( int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++ ) {
                Key key = Key( AXIS, i );
                Val val = Val( INVALID, VHat( -1, -1 ) );
                gameControllerToJoystick[ GUID ][ key ] = val;
                emit setMapping( GUID, keyToMappingString( key ), valToFriendlyString( val ) );
            }

            // TODO: Read value from disk
            analogToDpad[ GUID ] = false;
            dpadToAnalog[ GUID ] = false;

            // Set calibration flag, analog values will be read and deadzone values will be calculated the first time
            // we get data
            deadzoneFlag[ GUID ] = true;

            QString mappingString = gamepad.mappingString;

            if( mappingString.isEmpty() ) {
                return;
            }

            // Parse mapping string
            {
                QStringList mappings = mappingString.split( ',', QString::SkipEmptyParts );

                // No one likes QList asserts failing
                if( mappings.isEmpty() ) {
                    return;
                }

                /*QString gamepadGUID =*/ mappings.takeFirst();

                if( mappings.isEmpty() ) {
                    return;
                }

                QString friendlyName = mappings.takeFirst();

                if( mappings.isEmpty() ) {
                    return;
                }

                QString platform;

                // Parse the main part of the remapping string
                for( QString mapping : mappings ) {
                    if( mapping.contains( "platform" ) ) {
                        platform = mapping;
                        continue;
                    }

                    QString keyString = mapping.section( ':', 0, 0 );
                    QString valueString = mapping.section( ':', 1, 1 );

                    // Give up if there's nothing parsed
                    if( keyString.isEmpty() || valueString.isEmpty() ) {
                        continue;
                    }

                    Type valueType;

                    // Parse the key
                    Key key = Key( BUTTON, SDL_CONTROLLER_BUTTON_INVALID );
                    {
                        bool okay;
                        key = mappingStringToKey( keyString, &okay );

                        if( !okay ) {
                            continue;
                        }
                    }

                    // Parse the value
                    int value = -1;
                    int hatValue = -1;
                    {
                        // Button, format: bX
                        if( valueString.startsWith( "b" ) ) {
                            valueType = BUTTON;
                            bool okay;
                            value = valueString.right( valueString.size() - 1 ).toInt( &okay );

                            if( !okay ) {
                                continue;
                            }
                        }
                        // Hat, format: hX.Y
                        else if( valueString.startsWith( "h" ) ) {
                            valueType = HAT;
                            bool okay;
                            value = valueString.right( valueString.size() - 1 ).split( '.' )[ 0 ].toInt( &okay );

                            if( !okay ) {
                                continue;
                            }

                            hatValue = valueString.split( '.' )[ 1 ].toInt( &okay );

                            if( !okay ) {
                                continue;
                            }
                        }

                        // Axis, format aX
                        else if( valueString.startsWith( "a" ) ) {
                            valueType = AXIS;
                            bool okay;
                            value = valueString.right( valueString.size() - 1 ).toInt( &okay );

                            if( !okay ) {
                                continue;
                            }
                        } else {
                            continue;
                        }
                    }

                    //qDebug() << keyString << valueString << key << keyType << value << hatValue << valueType;
                    gameControllerToJoystick[ GUID ][ key ] = Val( valueType, VHat( value, hatValue ) );
                    emit setMapping( GUID, keyToMappingString( key ), valToFriendlyString( Val( valueType, VHat( value, hatValue ) ) ) );
                }
            }

            break;
        }

        case Command::RemoveController: {
            GamepadState gamepad = data.value<GamepadState>();
            int instanceID = gamepad.instanceID;
            QString GUID( QByteArray( reinterpret_cast<const char *>( gamepad.GUID.data ), 16 ).toHex() );
            GUIDCount[ GUID ]--;

            // Remove from map if no longer there
            if( GUIDCount[ GUID ] == 0 ) {
                GUIDCount.remove( GUID );
                emit controllerRemoved( GUID );
            }

            // If we opened an SDL2 Game controller handle from this class, keep it and inject it into all gamepads we send out
            if( gameControllerHandles[ instanceID ] ) {
                gamepad.gamecontrollerHandle = gameControllerHandles[ instanceID ];
            }

            gameControllerHandles.remove( instanceID );

            emit commandOut( Command::RemoveController, QVariant::fromValue( gamepad ), nodeCurrentTime() );
            break;
        }

        default: {
            emit commandOut( command, data, timeStamp );
            break;
        }
    }
}

void Remapper::dataIn( Node::DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) {
    switch( type ) {
        case DataType::Input: {
            // Copy incoming data to our own buffer
            GamepadState gamepad;
            {
                mutex->lock();
                gamepad = *reinterpret_cast<GamepadState *>( data );
                mutex->unlock();
            }

            int instanceID = gamepad.instanceID;
            int joystickID = gamepad.joystickID;
            QString GUID( QByteArray( reinterpret_cast<const char *>( gamepad.GUID.data ), 16 ).toHex() );

            // Do initial calibration if not done yet
            {
                if( deadzoneFlag[ GUID ] ) {
                    deadzoneFlag[ GUID ] = false;

                    // Apply default value
                    for( int i = 0; i < gamepad.joystickNumAxes; i++ ) {
                        deadzones[ GUID ][ i ] = 10000;

                        // Check analog value at this moment. If its magnitude is less than 30000 then it's most likely
                        // an analog stick. Otherwise, it might be a trigger (with a centered value of -32768)
                        deadzoneModes[ GUID ][ i ] = ( qAbs( static_cast<int>( gamepad.joystickAxis[ i ] ) ) < 30000 );
                    }

                    // TODO: Replace with stored value from disk
                }
            }

            // Inject deadzone settings into gamepad
            {
                for( int i = 0; i < gamepad.joystickNumAxes; i++ ) {
                    gamepad.deadzone[ i ] = deadzones[ GUID ][ i ];
                    gamepad.deadzoneMode[ i ] = deadzoneModes[ GUID ][ i ];
                }
            }

            // Send raw joystick data to the model
            // Do this before we apply joystick deadzones so the user sees completely unprocessed data
            {
                // Copy current gamepad into buffer
                this->mutex.lock();
                gamepadBuffer[ gamepadBufferIndex ] = gamepad;
                this->mutex.unlock();

                // Send buffer on its way
                emit rawJoystickData( &( this->mutex ), reinterpret_cast<void *>( &gamepadBuffer[ gamepadBufferIndex ] ) );

                // Increment the index
                gamepadBufferIndex = ( gamepadBufferIndex + 1 ) % 100;
            }

            // Apply deadzones to each stick and both triggers independently
            {
                qreal deadzone = 0.0;
                bool deadzoneMode = false;

                for( int i = 0; i < 4; i++ ) {
                    int xAxis;
                    int yAxis;

                    // For the analog sticks, average the underlying joystick axes together to get the final deadzone value
                    // If either axis has deadzone mode set to true, it'll apply to both
                    // FIXME: If users complain about this, expand the code to handle this case (one axis true and one axis false) and treat axes indepenently
                    switch( i ) {
                        case 0: {
                            xAxis = SDL_CONTROLLER_AXIS_LEFTX;
                            yAxis = SDL_CONTROLLER_AXIS_LEFTY;

                            Val val = gameControllerToJoystick[ GUID ][ Key( AXIS, SDL_CONTROLLER_AXIS_LEFTX ) ];
                            int axisID = val.second.first;

                            if( val.first == AXIS ) {
                                deadzone = deadzones[ GUID ][ axisID ];
                                deadzoneMode = deadzoneModes[ GUID ][ axisID ];
                            }

                            Val val2 = gameControllerToJoystick[ GUID ][ Key( AXIS, SDL_CONTROLLER_AXIS_LEFTY ) ];
                            axisID = val2.second.first;

                            if( val2.first == AXIS ) {
                                deadzone += deadzones[ GUID ][ axisID ];
                                deadzone /= 2.0;
                                deadzoneMode = deadzoneMode || deadzoneModes[ GUID ][ axisID ];
                            }

                            break;
                        }

                        case 1: {
                            xAxis = SDL_CONTROLLER_AXIS_RIGHTX;
                            yAxis = SDL_CONTROLLER_AXIS_RIGHTY;

                            Val val = gameControllerToJoystick[ GUID ][ Key( AXIS, SDL_CONTROLLER_AXIS_RIGHTX ) ];
                            int axisID = val.second.first;

                            if( val.first == AXIS ) {
                                deadzone = deadzones[ GUID ][ axisID ];
                                deadzoneMode = deadzoneModes[ GUID ][ axisID ];
                            }

                            Val val2 = gameControllerToJoystick[ GUID ][ Key( AXIS, SDL_CONTROLLER_AXIS_RIGHTY ) ];
                            axisID = val2.second.first;

                            if( val2.first == AXIS ) {
                                deadzone += deadzones[ GUID ][ axisID ];
                                deadzone /= 2.0;
                                deadzoneMode = deadzoneMode || deadzoneModes[ GUID ][ axisID ];
                            }

                            break;
                        }

                        // For simplicity, just map the triggers to the line y = x
                        case 2: {
                            xAxis = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
                            yAxis = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
                            Val val = gameControllerToJoystick[ GUID ][ Key( AXIS, SDL_CONTROLLER_AXIS_TRIGGERLEFT ) ];
                            int axisID = val.second.first;

                            if( val.first == AXIS ) {
                                deadzone = deadzones[ GUID ][ axisID ];
                                deadzoneMode = deadzoneModes[ GUID ][ axisID ];
                            }

                            break;
                        }

                        case 3: {
                            xAxis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
                            yAxis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
                            Val val = gameControllerToJoystick[ GUID ][ Key( AXIS, SDL_CONTROLLER_AXIS_TRIGGERRIGHT ) ];
                            int axisID = val.second.first;

                            if( val.first == AXIS ) {
                                deadzone = deadzones[ GUID ][ axisID ];
                                deadzoneMode = deadzoneModes[ GUID ][ axisID ];
                            }

                            break;
                        }
                    }

                    // Map from [-32768, 32767] to [0, 32767]
                    if( !deadzoneMode ) {
                        gamepad.axis[ xAxis ] /= 2;
                        gamepad.axis[ yAxis ] /= 2;
                        gamepad.axis[ xAxis ] += 16384;
                        gamepad.axis[ yAxis ] += 16384;
                    }

                    // Get axis coords in cartesian coords
                    // Bottom right is positive -> top right is positive
                    qreal xCoord = gamepad.axis[ xAxis ];
                    qreal yCoord = -gamepad.axis[ yAxis ];

                    // Get radius from center
                    QVector2D position( static_cast<float>( xCoord ), static_cast<float>( yCoord ) );
                    qreal radius = static_cast<qreal>( position.length() );

                    if( !( radius > deadzone ) ) {
                        gamepad.axis[ xAxis ] = 0;
                        gamepad.axis[ yAxis ] = 0;

                        if( xAxis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ) {
                            gamepad.digitalL2 = false;
                        }

                        if( xAxis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT ) {
                            gamepad.digitalR2 = false;
                        }
                    } else {
                        if( xAxis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ) {
                            gamepad.digitalL2 = true;
                        }

                        if( xAxis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT ) {
                            gamepad.digitalR2 = true;
                        }
                    }
                }
            }

            // Apply deadzones to all joystick axes
            // Used only when detecting input for remapping
            {
                for( int i = 0; i < 16; i++ ) {
                    qreal deadzoneRadius = deadzones[ GUID ][ i ];
                    qreal coord = gamepad.joystickAxis[ i ];

                    if( !deadzoneModes[ GUID ][ i ] ) {
                        coord += 32768;
                    }

                    if( !( qAbs( coord ) > deadzoneRadius ) ) {
                        gamepad.joystickAxis[ i ] = 0;
                    }
                }
            }

            // If ignoreMode is set, the user hasn't let go of the button they were remapping to
            // Do not let the button go through until they let go
            {
                if( ignoreMode && ignoreModeGUID == GUID && ignoreModeInstanceID == gamepad.instanceID ) {
                    if( ignoreModeVal.first == BUTTON ) {
                        if( gamepad.joystickButton[ ignoreModeVal.second.first ] == SDL_PRESSED ) {
                            gamepad.joystickButton[ ignoreModeVal.second.first ] = SDL_RELEASED;
                        } else {
                            ignoreMode = false;
                        }
                    } else if( ignoreModeVal.first == HAT ) {
                        if( gamepad.joystickHat[ ignoreModeVal.second.first ] != SDL_HAT_CENTERED ) {
                            gamepad.joystickHat[ ignoreModeVal.second.first ] = SDL_HAT_CENTERED;
                        } else {
                            ignoreMode = false;
                        }
                    } else if( ignoreModeVal.first == AXIS ) {
                        if( gamepad.joystickAxis[ ignoreModeVal.second.first ] != 0 ) {
                            gamepad.joystickAxis[ ignoreModeVal.second.first ] = 0;
                        } else {
                            ignoreMode = false;
                        }
                    }
                }
            }

            // If we opened an SDL2 Game controller handle from this class, keep it and inject it into all gamepads we send out
            {
                if( gameControllerHandles[ instanceID ] ) {
                    gamepad.gamecontrollerHandle = gameControllerHandles[ instanceID ];
                }
            }

            // If we are in remap mode, check for a button press from the stored GUID, and remap the stored button to that button
            // All game controller states are cleared past this point if in remap mode
            {
                if( remapMode && GUID == remapModeGUID ) {
                    // Find a button press, the first one we encounter will be the new remapping
                    bool foundInput = false;
                    Key key = remapModeKey;
                    Val value = Val( INVALID, VHat( -1, -1 ) );

                    // Prioritize buttons and hats over analog sticks
                    for( int joystickButton = 0; joystickButton < 256; joystickButton++ ) {
                        if( gamepad.joystickButton[ joystickButton ] == SDL_PRESSED ) {
                            qCDebug( phxInput ).nospace() << "Button b" << joystickButton
                                                          << " from GUID " << GUID << " now activates " << keyToMappingString( key );

                            foundInput = true;
                            value.first = BUTTON;
                            value.second = VHat( joystickButton, -1 );
                            break;
                        }
                    }

                    for( int joystickHat = 0; joystickHat < 16; joystickHat++ ) {
                        if( gamepad.joystickHat[ joystickHat ] != SDL_HAT_CENTERED ) {
                            qCDebug( phxInput ).nospace() << "Hat h" << joystickHat << "." << gamepad.joystickHat[ joystickHat ]
                                                          << " from GUID " << GUID << " now activates " << keyToMappingString( key );
                            foundInput = true;
                            value.first = HAT;
                            value.second = VHat( joystickHat, gamepad.joystickHat[ joystickHat ] );
                            break;
                        }
                    }

                    for( int joystickAxis = 0; joystickAxis < 16; joystickAxis++ ) {
                        if( gamepad.joystickAxis[ joystickAxis ] != 0 ) {
                            qCDebug( phxInput ).nospace() << "Axis a" << joystickAxis
                                                          << " from GUID " << GUID << " now activates " << keyToMappingString( key );
                            foundInput = true;
                            value.first = AXIS;
                            value.second = VHat( joystickAxis, -1 );
                            break;
                        }
                    }

                    if( foundInput ) {
                        // Store the new remapping internally
                        gameControllerToJoystick[ GUID ][ remapModeKey ] = value;

                        // Tell SDL2 about it
                        QString mappingString;
                        QString platform( SDL_GetPlatform() );
                        {
                            QString friendlyName = QString( SDL_JoystickName( gamepad.joystickHandle ) );

                            mappingString.append( GUID ).append( "," ).append( friendlyName ).append( "," );

                            for( Key key : gameControllerToJoystick[ GUID ].keys() ) {
                                if( gameControllerToJoystick[ GUID ][ key ].first != INVALID ) {
                                    mappingString.append( keyToMappingString( key ) ).append( ':' )
                                    .append( valToMappingString( gameControllerToJoystick[ GUID ][ key ] ) ).append( ',' );
                                }
                            }


                            mappingString.append( "platform:" ).append( platform ).append( "," );
                            qDebug().nospace() << mappingString;

                            // Give SDL the new mapping string
                            SDL_GameControllerAddMapping( mappingString.toUtf8().constData() );

                            // If this is not a game controller, reopen as one
                            SDL_GameController *gamecontrollerHandle = nullptr;

                            if( !gameControllerHandles[ instanceID ] ) {
                                gamecontrollerHandle = SDL_GameControllerOpen( joystickID );
                                gamepad.gamecontrollerHandle = gamecontrollerHandle;
                                // Store internally so we can inject it into all future events from this instanceID
                                gameControllerHandles[ instanceID ] = gamecontrollerHandle;
                                qDebug() << "Opened newly remapped joystick as a game controller:" << gamecontrollerHandle;
                            }
                        }

                        // Store this mapping to disk
                        {
                            if( !userDataPath.isEmpty() ) {
                                QFile mappingFile( userDataPath + "/gamecontrollerdb.txt" );

                                mappingFile.open( QIODevice::ReadWrite | QIODevice::Text );
                                QByteArray mappingFileData = mappingFile.readAll();

                                if( !mappingFile.isOpen() ) {
                                    qWarning() << "Unable to open mapping file for reading" << mappingFile.errorString();
                                }

                                mappingFile.close();

                                QTextStream mappingFileStreamIn( &mappingFileData );
                                mappingFileStreamIn.setCodec( "UTF-8" );

                                mappingFile.open( QIODevice::WriteOnly | QIODevice::Text );

                                if( !mappingFile.isOpen() ) {
                                    qWarning() << "Unable to open mapping file for writing" << mappingFile.errorString();
                                }

                                QTextStream mappingFileStreamOut( &mappingFile );
                                mappingFileStreamOut.setCodec( "UTF-8" );

                                QString line = "";

                                while( !line.isNull() ) {
                                    line = mappingFileStreamIn.readLine();

                                    // We want to replace the line (any line) for our platform that contains our GUID
                                    // We'll also filter out empty lines
                                    if( line.isEmpty() || ( line.contains( GUID ) && line.contains( platform ) ) ) {
                                        continue;
                                    }

                                    mappingFileStreamOut << line << endl;
                                }

                                mappingFileStreamOut << mappingString << endl;
                                mappingFile.close();
                            } else {
                                qWarning() << "Unable to open controller mapping file, user data path not set";
                            }
                        }

                        // End remap mode, start ignore mode
                        {
                            remapMode = false;
                            ignoreMode = true;
                            ignoreModeGUID = GUID;
                            ignoreModeVal = value;
                            ignoreModeInstanceID = gamepad.instanceID;
                        }

                        // Tell the model we're done
                        {
                            emit setMapping( GUID, keyToMappingString( key ), valToFriendlyString( value ) );
                            emit remappingEnded();
                        }
                    }

                    // Clear all game controller states (joystick states are untouched)
                    for( int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++ ) {
                        gamepad.button[ i ] = 0;
                    }

                    for( int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++ ) {
                        gamepad.axis[ i ] = 0;
                    }
                } else if( remapMode ) {
                    // Clear all gamepad states
                    for( int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++ ) {
                        gamepad.button[ i ] = 0;
                    }

                    for( int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++ ) {
                        gamepad.axis[ i ] = 0;
                    }
                }
            }

            // OR all joystick button, hat and analog states together by GUID for RemapperModel to indicate presses
            {
                for( int i = 0; i < 256; i++ ) {
                    pressed[ GUID ] |= gamepad.joystickButton[ i ];
                }

                for( int i = 0; i < 16; i++ ) {
                    pressed[ GUID ] |= ( gamepad.joystickHat[ i ] != SDL_HAT_CENTERED );
                }

                for( int i = 0; i < 16; i++ ) {
                    pressed[ GUID ] |= ( gamepad.joystickAxis[ i ] != 0 );
                }
            }

            // Apply axis to d-pad, if enabled
            // This will always be enabled if we're not currently playing so GlobalGamepad can use the analog stick
            {
                if( analogToDpad[ GUID ] || !playing ) {
                    // TODO: Support other axes?
                    int xAxis = SDL_CONTROLLER_AXIS_LEFTX;
                    int yAxis = SDL_CONTROLLER_AXIS_LEFTY;

                    // TODO: Let user configure these

                    qreal threshold = 16384.0;

                    // Size in degrees of the arc covering and centered around each cardinal direction
                    // If <90, there will be gaps in the diagonals
                    // If >180, this code will always produce diagonal inputs
                    qreal rangeDegrees = 180.0 - 45.0;

                    // Get axis coords in cartesian coords
                    // Bottom right is positive -> top right is positive
                    qreal xCoord = gamepad.axis[ xAxis ];
                    qreal yCoord = -gamepad.axis[ yAxis ];

                    // Get radius from center
                    QVector2D position( static_cast<float>( xCoord ), static_cast<float>( yCoord ) );
                    qreal radius = static_cast<qreal>( position.length() );

                    // Get angle in degrees
                    qreal angle = qRadiansToDegrees( qAtan2( yCoord, xCoord ) );

                    if( angle < 0.0 ) {
                        angle += 360.0;
                    }

                    if( radius > threshold ) {
                        qreal halfRange = rangeDegrees / 2.0;

                        if( angle > 90.0 - halfRange && angle < 90.0 + halfRange ) {
                            gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_UP ] = true;
                        }

                        if( angle > 270.0 - halfRange && angle < 270.0 + halfRange ) {
                            gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_DOWN ] = true;
                        }

                        if( angle > 180.0 - halfRange && angle < 180.0 + halfRange ) {
                            gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_LEFT ] = true;
                        }

                        if( angle > 360.0 - halfRange || angle < 0.0 + halfRange ) {
                            gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_RIGHT ] = true;
                        }
                    }
                }
            }

            // Apply d-pad to axis, if enabled
            {
                if( dpadToAnalog[ GUID ] ) {
                    gamepad = mapDpadToAnalog( gamepad );
                }
            }

            // Send updated data out
            {
                // Copy current gamepad into buffer
                this->mutex.lock();
                gamepadBuffer[ gamepadBufferIndex ] = gamepad;
                this->mutex.unlock();

                // Send buffer on its way
                emit dataOut( DataType::Input, &( this->mutex ),
                              reinterpret_cast<void *>( &gamepadBuffer[ gamepadBufferIndex ] ), 0,
                              nodeCurrentTime() );

                // Increment the index
                gamepadBufferIndex = ( gamepadBufferIndex + 1 ) % 100;
            }
            break;
        }

        case DataType::KeyboardInput: {
            // Unpack keyboard states and write to gamepad according to remap data
            {
                mutex->lock();
                KeyboardState keyboard = *reinterpret_cast<KeyboardState *>( data );

                for( int i = keyboard.head; i < keyboard.tail; i = ( i + 1 ) % 128 ) {
                    int key = keyboard.key[ i ];
                    bool pressed = keyboard.pressed[ i ];

                    if( keyboardKeyToSDLButton.contains( key ) ) {
                        keyboardGamepad.button[ keyboardKeyToSDLButton[ key ] ] = pressed ? SDL_PRESSED : SDL_RELEASED;
                    }
                }

                mutex->unlock();
            }

            // OR all key states together and store that value
            for( int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++ ) {
                keyboardKeyPressed |= keyboardGamepad.button[ i ];
            }

            // Apply d-pad to axis, if enabled
            if( dpadToAnalogKeyboard ) {
                keyboardGamepad = mapDpadToAnalog( keyboardGamepad, true );
            }

            // Send gamepad on its way
            {
                // Copy current gamepad into buffer
                this->mutex.lock();
                gamepadBuffer[ gamepadBufferIndex ] = keyboardGamepad;
                this->mutex.unlock();

                // Send buffer on its way
                emit dataOut( DataType::Input, &( this->mutex ),
                              reinterpret_cast< void * >( &gamepadBuffer[ gamepadBufferIndex ] ), 0,
                              nodeCurrentTime() );

                // Increment the index
                gamepadBufferIndex = ( gamepadBufferIndex + 1 ) % 100;
            }
            break;
        }

        default: {
            emit dataOut( type, mutex, data, bytes, timeStamp );
            break;
        }
    }
}

void Remapper::beginRemapping( QString GUID, QString button ) {
    // TODO: Let the user know about this situation
    // FIXME: Wouldn't muting problem buttons fix this? Is this even necessary? Should we detect conflicts and offer to mute?
    if( ignoreMode ) {
        Key key = mappingStringToKey( button );
        Val value = gameControllerToJoystick[ GUID ][ key ];
        qInfo().noquote() << "Release" << valToFriendlyString( ignoreModeVal ) << "first before trying to remap" << button;
        emit setMapping( GUID, button, valToFriendlyString( value ) );
        emit remappingEnded();
        return;
    }

    remapMode = true;
    remapModeGUID = GUID;
    remapModeKey = mappingStringToKey( button );
}

void Remapper::setDeadzone( QString GUID, int axis, Sint16 deadzone, bool deadzoneMode ) {
    deadzones[ GUID ][ axis ] = deadzone;
    deadzoneModes[ GUID ][ axis ] = deadzoneMode;
}

// Private

GamepadState Remapper::mapDpadToAnalog( GamepadState gamepad, bool clear ) {
    // TODO: Support other axes?
    // TODO: Support multiple axes?
    int xAxis = SDL_CONTROLLER_AXIS_LEFTX;
    int yAxis = SDL_CONTROLLER_AXIS_LEFTY;

    // The distance from center the analog stick will go if a d-pad button is pressed
    // TODO: Let the user set this
    // TODO: Recommend to the user to set this to a number comparable to the analog stick's actual range
    qreal maxRange = 32768.0;

    bool up = gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_UP ] == SDL_PRESSED;
    bool down = gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_DOWN ] == SDL_PRESSED;
    bool left = gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_LEFT ] == SDL_PRESSED;
    bool right = gamepad.button[ SDL_CONTROLLER_BUTTON_DPAD_RIGHT ] == SDL_PRESSED;

    if( up || down || left || right ) {
        qreal angle = 0.0;

        // Check diagonals first
        if( up && right ) {
            angle = 45.0;
        } else if( up && left ) {
            angle = 135.0;
        } else if( down && left ) {
            angle = 225.0;
        } else if( down && right ) {
            angle = 315.0;
        } else if( right ) {
            angle = 0.0;
        } else if( up ) {
            angle = 90.0;
        } else if( left ) {
            angle = 180.0;
        } else { /*if( down )*/
            angle = 270.0;
        }

        // Get coords on a unit circle
        qreal xScale = qCos( qDegreesToRadians( angle ) );
        qreal yScale = qSin( qDegreesToRadians( angle ) );

        // Convert from positive top right coord system to positive bottom right coord system
        yScale = -yScale;

        // Map unit circle range to full range of Sint16 without over/underflowing
        Sint16 xValue = 0;
        Sint16 yValue = 0;
        {
            // Map scales from [-1.0, 1.0] to [0.0, 1.0]
            xScale += 1.0;
            xScale /= 2.0;
            yScale += 1.0;
            yScale /= 2.0;

            // Map scales from [0.0, 1.0] to [0, maxRange + maxRange - 1]
            xScale *= maxRange + maxRange - 1;
            yScale *= maxRange + maxRange - 1;

            // Map scales from [0, maxRange + maxRange - 1] to [-maxRange, maxRange - 1]
            xScale -= maxRange;
            yScale -= maxRange;

            // Convert to int (drops fractional value, not the same as a floor operation!)
            xValue = static_cast<Sint16>( xScale );
            yValue = static_cast<Sint16>( yScale );
        }

        // Finally, set the value and return
        gamepad.axis[ xAxis ] = xValue;
        gamepad.axis[ yAxis ] = yValue;
    } else if( clear ) {
        gamepad.axis[ xAxis ] = 0;
        gamepad.axis[ yAxis ] = 0;
    }

    return gamepad;
}

// FIXME: Kinda overlaps with keyToMappingString()
QString gameControllerIDToMappingString( int gameControllerID ) {
    QString result = QString( SDL_GameControllerGetStringForButton( static_cast<SDL_GameControllerButton>( gameControllerID ) ) );

    if( !result.isEmpty() ) {
        return result;
    }

    result = QString( SDL_GameControllerGetStringForAxis( static_cast<SDL_GameControllerAxis>( gameControllerID ) ) );

    // Will return either a valid label or empty string
    return result;
}

int mappingStringToGameControllerID( QString gameControllerString ) {
    int result;
    result = SDL_GameControllerGetButtonFromString( gameControllerString.toUtf8().constData() );

    if( result != SDL_CONTROLLER_BUTTON_INVALID ) {
        return result;
    }

    result = SDL_GameControllerGetAxisFromString( gameControllerString.toUtf8().constData() );

    // Will return either a valid axis or SDL_CONTROLLER_AXIS_INVALID
    return result;
}

QString keyToMappingString( Remapper::Key key ) {
    QString keyString;

    if( key.first == Remapper::BUTTON ) {
        keyString.append( SDL_GameControllerGetStringForButton( static_cast<SDL_GameControllerButton>( key.second ) ) );
    } else if( key.first == Remapper::AXIS ) {
        keyString.append( SDL_GameControllerGetStringForAxis( static_cast<SDL_GameControllerAxis>( key.second ) ) );
    }

    return keyString;
}

QString valToFriendlyString( Remapper::Val val ) {
    QString valString = "(unmapped)";

    if( val.first == Remapper::BUTTON ) {
        valString.clear();
        valString.append( "Button " ).append( QString( "%1" ).arg( val.second.first ) );
    } else if( val.first == Remapper::AXIS ) {
        valString.clear();
        valString.append( "Axis " ).append( QString( "%1" ).arg( val.second.first ) );
    } else if( val.first == Remapper::HAT ) {
        valString.clear();
        valString.append( "Hat " ).append( QString( "%1 direction %2" ).arg( val.second.first ).arg( val.second.second ) );
    }

    return valString;
}

Remapper::Key mappingStringToKey( QString keyString, bool *ok ) {
    Remapper::Type keyType;
    int keyValue;

    bool okay = false;

    // Try to parse the key as a button
    keyValue = SDL_GameControllerGetButtonFromString( keyString.toLocal8Bit().constData() );

    okay = true;
    keyType = Remapper::BUTTON;

    // If it's not a button then it's an axis
    if( keyValue == SDL_CONTROLLER_BUTTON_INVALID ) {
        keyValue = SDL_GameControllerGetAxisFromString( keyString.toLocal8Bit().constData() );
        keyType = Remapper::AXIS;
    }

    // If it's not that then what the heck is it?
    if( keyValue == SDL_CONTROLLER_AXIS_INVALID ) {
        okay = false;
    }

    if( ok ) {
        *ok = okay;
    }

    return Remapper::Key( keyType, keyValue );
}

QString valToMappingString( Remapper::Val val ) {
    QString valString = "ERROR";

    if( val.first == Remapper::BUTTON ) {
        valString.clear();
        valString.append( "b" ).append( QString( "%1" ).arg( val.second.first ) );
    } else if( val.first == Remapper::AXIS ) {
        valString.clear();
        valString.append( "a" ).append( QString( "%1" ).arg( val.second.first ) );
    } else if( val.first == Remapper::HAT ) {
        valString.clear();
        valString.append( "h" ).append( QString( "%1.%2" ).arg( val.second.first ).arg( val.second.second ) );
    }

    return valString;
}
