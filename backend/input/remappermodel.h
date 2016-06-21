#pragma once

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include <iterator>

#include "gamepadstate.h"
#include "node.h"

/*
 * RemapperModel is a QML model whose job is to relay information between the QML world and Remapper, which is part
 * of the global pipeline. It provides a list of all controllers currently connected (indexed internally by joystickID)
 * with the following roles:
 * name (QString): Name of the controller. By default it will use the SDL-provided name.
 * GUID (QString): Controller GUID
 * pressed (bool): True if any button on this controller is currently being pressed. Used so the user can tell controllers
 *                 of the same model apart.
 * remapData (map): Map of string:string, key is the button name and the value is the remapped name
 */

class Remapper;

class RemapperModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY( bool remapMode MEMBER remapMode NOTIFY remapModeChanged )

    public:
        explicit RemapperModel( QAbstractListModel *parent = nullptr );

        enum RemapperRoles {
            // GUID: String
            // GUID of the controller
            GUIDRole = Qt::UserRole + 1,

            // remapData: Object( String:String )
            // Remap data for this GUID ( button:button )
            RemapDataRole,

            // available: Boolean
            // True iff a controller is plugged in for this particular GUID
            AvailableRole,

            // pressed: Boolean
            // True iff any controller with this GUID has any button pressed
            PressedRole,

            // friendlyName: String
            // Friendly name of the controller as provided by SDL
            FriendlyNameRole,

            // button: Array[ Boolean ]
            // Indexed by buttonID
            ButtonRole,

            // hat: Array[ Number ]
            // Indexed by hatID
            // Valid values:
            /*
             * #define SDL_HAT_CENTERED    0x00
             * #define SDL_HAT_UP          0x01
             * #define SDL_HAT_RIGHT       0x02
             * #define SDL_HAT_DOWN        0x04
             * #define SDL_HAT_LEFT        0x08
             * #define SDL_HAT_RIGHTUP     (SDL_HAT_RIGHT|SDL_HAT_UP)
             * #define SDL_HAT_RIGHTDOWN   (SDL_HAT_RIGHT|SDL_HAT_DOWN)
             * #define SDL_HAT_LEFTUP      (SDL_HAT_LEFT|SDL_HAT_UP)
             * #define SDL_HAT_LEFTDOWN    (SDL_HAT_LEFT|SDL_HAT_DOWN)
             */
            HatRole,

            // axis: Array[ Number ]
            // Indexed by axisID
            // Range: -32768 to 32767
            AxisRole,

            // numButtons: Number
            // Number of available buttonIDs, used to index into "button"
            NumButtonsRole,

            // numHats: Number
            // Number of available hatIDs, used to index into "hat"
            NumHatsRole,

            // numAxes: Number
            // Number of available axisIDs, used to index into "axis"
            NumAxesRole,

            // deadzone: Number
            // Minimum magnitude necessary to register an analog tilt
            DeadzoneRole,

            // deadzoneMode: Boolean
            // True: Measure magnitude relative to 0
            // False: Measure magnitude relative to -32768
            DeadzoneModeRole,
        };

        QHash<int, QByteArray> roleNames() const override;

        QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

        // One row per GUID
        int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    signals:
        void remapModeChanged();
        void beginRemappingProxy( QString GUID, QString button );
        void setDeadzoneProxy( QString GUID, int axis, int deadzone, bool deadzoneMode );

    public slots:
        // A new controller GUID was seen, add to the model's list
        void controllerAdded( QString GUID, QString friendlyName , int numButtons, int numHats, int numAxes );

        // The last remaining controller with this GUID was removed, do not accept remap requests for this one
        void controllerRemoved( QString GUID );

        // Only fired while not in remap mode, true if any button on any controller with this GUID has been pressed
        void buttonUpdate( QString GUID, bool pressed );

        // Remap mode completed, update UI
        void remappingEnded();

        // Update remapData, reset model
        void setMapping( QString GUID, QString physicalButton , QString virtualButton );

        // Connect to this Remapper
        void setRemapper( Remapper *t_remapper );

        // Call from QML
        void beginRemapping( QString GUID, QString button );
        void setDeadzone( QString GUID, int axis, int deadzone, bool deadzoneMode );

        // Used for displaying raw joystick data to the user
        void heartbeat();
        void rawJoystickData( QMutex *mutex, void *data );

    private:
        // Are we currently remapping?
        bool remapMode{ false };

        // Copy of the remapping data
        // GUID : ( physicalButton:virtualButton )
        QMap<QString, QStringMap> remapData;

        // True if any controller with a particular GUID has any button pressed
        // GUID : button pressed
        QMap<QString, bool> pressed;

        // True if any controller is plugged in with this GUID
        QMap<QString, bool> available;

        // Friendly names for controllers as provided by SDL
        QStringMap friendlyNames;

        // Our copy of all the gamepads currently available
        // Indexed by GUID which means each entry is an OR of all connected gamepads with the same GUID
        // GUID : gamepad
        QMap<QString, GamepadState> gamepads;

        // Helpers

        bool insertRowIfNotPresent( QString GUID );

        // Returns the next highest GUID if the given one does not exist
        int GUIDToRow( QString GUID ) const;

        QString rowToGUID( int row ) const;
};
