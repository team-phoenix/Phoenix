#include "remappermodel.h"
#include "remapper.h"

RemapperModel::RemapperModel( QAbstractListModel *parent ) : QAbstractListModel( parent ) {

}

// Public

QHash<int, QByteArray> RemapperModel::roleNames() const {
    return {
        { GUIDRole, QByteArrayLiteral( "GUID" ) },
        { RemapDataRole, QByteArrayLiteral( "remapData" ) },
        { AvailableRole, QByteArrayLiteral( "available" ) },
        { PressedRole, QByteArrayLiteral( "pressed" ) },
        { FriendlyNameRole, QByteArrayLiteral( "friendlyName" ) },
        { ButtonRole, QByteArrayLiteral( "button" ) },
        { HatRole, QByteArrayLiteral( "hat" ) },
        { AxisRole, QByteArrayLiteral( "axis" ) },
        { NumButtonsRole, QByteArrayLiteral( "numButtons" ) },
        { NumHatsRole, QByteArrayLiteral( "numHats" ) },
        { NumAxesRole, QByteArrayLiteral( "numAxes" ) },
        { DeadzoneRole, QByteArrayLiteral( "deadzone" ) },
        { DeadzoneModeRole, QByteArrayLiteral( "deadzoneMode" ) },
    };
}

QVariant RemapperModel::data( const QModelIndex &index, int role ) const {
    // Sanity checks
    {
        if( remapData.isEmpty() ) {
            return QVariant();
        }

        if( index.row() < 0 || index.row() >= remapData.size() ) {
            return QVariant();
        }
    }

    switch( role ) {
        case GUIDRole: {
            QString GUID = rowToGUID( index.row() );
            return GUID;
        }

        case RemapDataRole: {
            // Get the GUID
            QString GUID = rowToGUID( index.row() );

            // Get the remap data
            QVariantMap remapData;

            // Generate virtualButton -> physicalButtons for the keyboard
            if( GUID.isEmpty() ) {
                for( int virtualButton = 0; virtualButton < SDL_CONTROLLER_BUTTON_MAX; virtualButton++ ) {
                    remapData[ gameControllerIDToMappingString( virtualButton ) ] = QStringList();
                    QStringList list;

                    for( QString physicalButton : this->remapData[ GUID ].keys() ) {
                        if( this->remapData[ GUID ][ physicalButton ] == gameControllerIDToMappingString( virtualButton ) ) {
                            list.append( physicalButton );
                        }
                    }

                    remapData[ gameControllerIDToMappingString( virtualButton ) ].setValue( list );
                }
            }

            // Convert QMap<QString, QString> to QMap<QString, QVariant> for controllers
            else {
                for( QString key : this->remapData[ GUID ].keys() ) {
                    remapData[ key ] = this->remapData[ GUID ][ key ];
                }
            }

            return remapData;
        }

        case AvailableRole: {
            return available[ rowToGUID( index.row() ) ];
        }

        case PressedRole: {
            return pressed[ rowToGUID( index.row() ) ];
        }

        case FriendlyNameRole: {
            return friendlyNames[ rowToGUID( index.row() ) ];
        }

        case ButtonRole: {
            // Build a list
            QList<QVariant> buttonStates;
            QString GUID = rowToGUID( index.row() );
            int numButtons = gamepads[ GUID ].joystickNumButtons;

            for( int joystickButton = 0; joystickButton < numButtons; joystickButton++ ) {
                buttonStates.append( gamepads[ GUID ].joystickButton[ joystickButton ] );
            }

            // Convert to a QVariant
            QVariant list = QVariant::fromValue( buttonStates );
            return list;
        }

        case HatRole: {
            // Build a list
            QList<QVariant> hatStates;
            QString GUID = rowToGUID( index.row() );
            int numHats = gamepads[ GUID ].joystickNumHats;

            for( int joystickHat = 0; joystickHat < numHats; joystickHat++ ) {
                hatStates.append( gamepads[ GUID ].joystickHat[ joystickHat ] );
            }

            // Convert to a QVariant
            QVariant list = QVariant::fromValue( hatStates );
            return list;
        }

        case AxisRole: {
            // Build a list
            QList<QVariant> axisStates;
            QString GUID = rowToGUID( index.row() );
            int numAxes = gamepads[ GUID ].joystickNumAxes;

            for( int joystickAxis = 0; joystickAxis < numAxes; joystickAxis++ ) {
                axisStates.append( gamepads[ GUID ].joystickAxis[ joystickAxis ] );
            }

            // Convert to a QVariant
            QVariant list = QVariant::fromValue( axisStates );
            return list;
        }

        case NumButtonsRole: {
            return gamepads[ rowToGUID( index.row() ) ].joystickNumButtons;
        }

        case NumHatsRole: {
            return gamepads[ rowToGUID( index.row() ) ].joystickNumHats;
        }

        case NumAxesRole: {
            return gamepads[ rowToGUID( index.row() ) ].joystickNumAxes;
        }

        case DeadzoneRole: {
            // Build a list
            QList<QVariant> axisDeadzones;
            QString GUID = rowToGUID( index.row() );
            int numAxes = gamepads[ GUID ].joystickNumAxes;

            for( int joystickAxis = 0; joystickAxis < numAxes; joystickAxis++ ) {
                axisDeadzones.append( gamepads[ GUID ].deadzone[ joystickAxis ] );
            }

            // Convert to a QVariant
            QVariant list = QVariant::fromValue( axisDeadzones );
            return list;
        };

        case DeadzoneModeRole: {
            // Build a list
            QList<QVariant> axisDeadzoneModes;
            QString GUID = rowToGUID( index.row() );
            int numAxes = gamepads[ GUID ].joystickNumAxes;

            for( int joystickAxis = 0; joystickAxis < numAxes; joystickAxis++ ) {
                axisDeadzoneModes.append( gamepads[ GUID ].deadzoneMode[ joystickAxis ] );
            }

            // Convert to a QVariant
            QVariant list = QVariant::fromValue( axisDeadzoneModes );
            return list;
        };

        default:
            return QVariant();
    }
}

int RemapperModel::rowCount( const QModelIndex & ) const {
    return remapData.size();
}

// Public slots

void RemapperModel::controllerAdded( QString GUID, QString friendlyName, int numButtons, int numHats, int numAxes ) {
    insertRowIfNotPresent( GUID );
    available[ GUID ] = true;
    friendlyNames[ GUID ] = friendlyName;
    gamepads[ GUID ].joystickNumButtons = numButtons;
    gamepads[ GUID ].joystickNumHats = numHats;
    gamepads[ GUID ].joystickNumAxes = numAxes;
    int row = GUIDToRow( GUID );
    emit dataChanged( createIndex( row, 0 ), createIndex( row, 0 ), { AvailableRole, FriendlyNameRole, NumButtonsRole, NumHatsRole, NumAxesRole, DeadzoneRole, DeadzoneModeRole } );
}

void RemapperModel::controllerRemoved( QString GUID ) {
    available[ GUID ] = false;
    int row = GUIDToRow( GUID );
    emit dataChanged( createIndex( row, 0 ), createIndex( row, 0 ), { AvailableRole } );
}

void RemapperModel::buttonUpdate( QString GUID, bool pressed ) {
    if( this->pressed[ GUID ] != pressed ) {
        this->pressed[ GUID ] = pressed;
        int row = GUIDToRow( GUID );
        emit dataChanged( createIndex( row, 0 ), createIndex( row, 0 ), { PressedRole } );
    }
}

void RemapperModel::remappingEnded() {
    remapMode = false;
    emit remapModeChanged();
}

void RemapperModel::setMapping( QString GUID, QString physicalButton, QString virtualButton ) {
    // Update the model if this GUID is not in the list yet
    insertRowIfNotPresent( GUID );

    // Add to/update the map
    remapData[ GUID ][ physicalButton ] = virtualButton;
    int row = GUIDToRow( GUID );
    emit dataChanged( createIndex( row, 0 ), createIndex( row, 0 ), { RemapDataRole } );
}

void RemapperModel::setRemapper( Remapper *t_remapper ) {
    connect( t_remapper, &Remapper::controllerAdded, this, &RemapperModel::controllerAdded );
    connect( t_remapper, &Remapper::controllerRemoved, this, &RemapperModel::controllerRemoved );
    connect( t_remapper, &Remapper::buttonUpdate, this, &RemapperModel::buttonUpdate );
    connect( t_remapper, &Remapper::remappingEnded, this, &RemapperModel::remappingEnded );
    connect( t_remapper, &Remapper::setMapping, this, &RemapperModel::setMapping );
    connect( t_remapper, &Remapper::rawJoystickData, this, &RemapperModel::rawJoystickData );
    connect( t_remapper, &Remapper::heartbeat, this, &RemapperModel::heartbeat );

    connect( this, &RemapperModel::beginRemappingProxy, t_remapper, &Remapper::beginRemapping );
    connect( this, &RemapperModel::setDeadzoneProxy, t_remapper, &Remapper::setDeadzone );
}

void RemapperModel::beginRemapping( QString GUID, QString button ) {
    if( !remapMode ) {
        remapMode = true;
        emit remapModeChanged();
        emit beginRemappingProxy( GUID, button );
    } else {
        qInfo() << "Remap mode already active!";
    }
}

void RemapperModel::setDeadzone( QString GUID, int axis, int deadzone, bool deadzoneMode ) {
    emit setDeadzoneProxy( GUID, axis, deadzone, deadzoneMode );
}

void RemapperModel::heartbeat() {
    // Clear all joystick states
    for( GamepadState &gamepad : gamepads ) {
        int numButtons = gamepad.joystickNumButtons;
        int numHats = gamepad.joystickNumHats;
        int numAxes = gamepad.joystickNumAxes;

        for( int joystickButton = 0; joystickButton < numButtons; joystickButton++ ) {
            gamepad.joystickButton[ joystickButton ] = 0;
        }

        for( int joystickHat = 0; joystickHat < numHats; joystickHat++ ) {
            gamepad.joystickHat[ joystickHat ] = 0;
        }

        for( int joystickAxis = 0; joystickAxis < numAxes; joystickAxis++ ) {
            gamepad.joystickAxis[ joystickAxis ] = 0;
        }
    }
}

void RemapperModel::rawJoystickData( QMutex *mutex, void *data ) {
    // Get gamepad state
    GamepadState gamepad;
    {
        mutex->lock();
        gamepad = *reinterpret_cast<GamepadState *>( data );
        mutex->unlock();
    }

    QString GUID = gamepad.GUIDString;

    // Check if anything's changed
    // FIXME: Broken, dirty always true, needs redesign
    GamepadState oldGamepad = gamepads[ GUID ];
    bool dirty = false;
    {
        int numButtons = gamepad.joystickNumButtons;
        int numHats = gamepad.joystickNumHats;
        int numAxes = gamepad.joystickNumAxes;

        for( int joystickButton = 0; joystickButton < numButtons; joystickButton++ ) {
            if( gamepad.joystickButton[ joystickButton ] != oldGamepad.joystickButton[ joystickButton ] ) {
                dirty = true;
                break;
            }
        }

        for( int joystickHat = 0; joystickHat < numHats; joystickHat++ ) {
            if( gamepad.joystickHat[ joystickHat ] != oldGamepad.joystickHat[ joystickHat ] ) {
                dirty = true;
                break;
            }
        }

        for( int joystickAxis = 0; joystickAxis < numAxes; joystickAxis++ ) {
            if( gamepad.joystickAxis[ joystickAxis ] != oldGamepad.joystickAxis[ joystickAxis ] ) {
                dirty = true;
                break;
            }
        }
    }

    // If things did change, store the new change and get the view to pull the new changes
    if( dirty ) {
        for( int joystickButton = 0; joystickButton < 256; joystickButton++ ) {
            // OR button states
            gamepads[ GUID ].joystickButton[ joystickButton ] |= gamepad.joystickButton[ joystickButton ];
        }

        for( int joystickHat = 0; joystickHat < 16; joystickHat++ ) {
            // OR hat states
            gamepads[ GUID ].joystickHat[ joystickHat ] |= gamepad.joystickHat[ joystickHat ];
        }

        for( int joystickAxis = 0; joystickAxis < 16; joystickAxis++ ) {
            gamepads[ GUID ].deadzone[ joystickAxis ] = gamepad.deadzone[ joystickAxis ];
            gamepads[ GUID ].deadzoneMode[ joystickAxis ] = gamepad.deadzoneMode[ joystickAxis ];

            // Over/underflow checks
            if( ( gamepads[ GUID ].joystickAxis[ joystickAxis ] + gamepad.joystickAxis[ joystickAxis ] > 32767 ) ) {
                gamepads[ GUID ].joystickAxis[ joystickAxis ] = 32767;
                continue;
            }

            if( ( gamepads[ GUID ].joystickAxis[ joystickAxis ] + gamepad.joystickAxis[ joystickAxis ] < -32768 ) ) {
                gamepads[ GUID ].joystickAxis[ joystickAxis ] = -32768;
                continue;
            }

            // Add axes together
            gamepads[ GUID ].joystickAxis[ joystickAxis ] += gamepad.joystickAxis[ joystickAxis ];
        }

        //qDebug() << "Changed, emitting changed row";
        int row = GUIDToRow( GUID );
        emit dataChanged( createIndex( row, 0 ), createIndex( row, 0 ), { ButtonRole, HatRole, AxisRole, DeadzoneRole, DeadzoneModeRole } );
    }
}

// Helpers

bool RemapperModel::insertRowIfNotPresent( QString GUID ) {
    if( !remapData.contains( GUID ) ) {
        // Find this GUID's position in the sorted list
        int row = GUIDToRow( GUID );

        // Place into list
        beginInsertRows( QModelIndex(), row, row );
        remapData.insert( GUID, QStringMap() );
        endInsertRows();
    }

    return false;
}

int RemapperModel::GUIDToRow( QString GUID ) const {
    return static_cast<int>( std::distance( remapData.begin(), remapData.lowerBound( GUID ) ) );
}

QString RemapperModel::rowToGUID( int row ) const {
    return remapData.keys()[ row ];
}
