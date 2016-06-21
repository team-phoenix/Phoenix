#pragma once

#include <QMutex>
#include <QObject>
#include <QHash>
#include <QMap>
#include <QPair>
#include <Qt>

#include "SDL.h"
#include "SDL_gamecontroller.h"

#include "node.h"
#include "gamepadstate.h"
#include "keyboardstate.h"
#include "logging.h"

typedef QMap<QString, QString> QStringMap;

/*
 * Remapper is a Node whose job is to transform input data that passes through it based on stored remapping data.
 * It also provides signals and slots to communicate with a QML-based frontend that will present the user with a
 * graphical interface for setting remappings.
 *
 * The remapper supports the button layout of an Xbox 360 controller (these are the actual strings used):
 *  Face buttons: A B X Y
 *  Center buttons: Back Guide Start
 *  DPAD: Up Down Left Right
 *  Bumpers: L R
 *  Stick buttons: L3 R3
 *
 * Note that L2 and R2 are not directly remappable, they're analog on the Xbox 360 controller (TODO?)
 *
 * The remapper also lets the user remap the keyboard. (TODO)
 *
 * One thing to note is that when we map an unmapped controller and open an SDL2 Game controller handle, it won't find
 * its way upstream to SDLManager's gamepad list as that's not allowed. Instead, we'll store it in this class
 * and inject it into every GamepadState we get that we're responsible for.
 */

class Remapper : public Node {
        Q_OBJECT

    public:
        Remapper();

        // Remap type
        enum Type { INVALID, BUTTON, AXIS, HAT };
        Q_ENUM( Type )

        // Encapsulates the mapping part a single GUID's remapping string
        // ex. "a:b0" becomes Mapping( Key( BUTTON, SDL_CONTROLLER_BUTTON_A ), Val( BUTTON, VHat( 0, -1 ) ) )
        // ex. "dpup:h0.1" becomes Mapping( Key( BUTTON, SDL_CONTROLLER_BUTTON_DPAD_UP ), Val( HAT, VHat( 0, 1 ) ) )
        // Key:   SDL2 Game controller (type, buttonID or axisID)
        // Value: SDL2 Joystick        (type, (buttonID or axisID or hatID, hat value))
        using Key = QPair<Type, int>;
        using VHat = QPair<int, int>;
        using Val = QPair<Type, VHat>;
        using Mapping = QMap<Key, Val>;

    signals:
        // Signals for RemapperModel

        // A new controller GUID was seen, add to the model's list
        void controllerAdded( QString GUID, QString friendlyName, int numButtons, int numHats, int numAxes );

        // The last remaining controller with this GUID was removed, do not accept remap requests for this one
        void controllerRemoved( QString GUID );

        // Only fired while not in remap mode, true if any button on any controller with this GUID has been pressed
        void buttonUpdate( QString GUID, bool pressed );

        // Fresh raw joystick states incoming, clear what you have
        void heartbeat();

        // Raw data meant to be displayed to the user
        void rawJoystickData( QMutex *mutex, void *data );

        // Remap mode completed, update the UI
        void remappingEnded();

        // Data for the model so it can update its internal copy of remapData
        void setMapping( QString GUID, QString physicalButton, QString virtualButton );

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;

        void dataIn( DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) override;

        // Slots from RemapperModel

        // Do not send input updates to this node's children, listen for a button press on this GUID and reply back
        // with a button once pressed
        void beginRemapping( QString GUID, QString button );

        void setDeadzone( QString GUID, int axis, Sint16 deadzone, bool deadzoneMode );

    private:
        // Node stuff

        // True iff we're playing right now
        bool playing{ false };

        QString userDataPath;

        // Producer stuff

        // Ensure reads and writes to gamepadBuffer are atomic
        QMutex mutex;

        // A circular buffer that holds gamepad state updates
        // A value of 100 should be sufficient for most purposes
        GamepadState gamepadBuffer[ 100 ];
        int gamepadBufferIndex { 0 };

        // Used to track which GUIDs have a button pressed
        // Cleared on each heartbeat
        QMap<QString, bool> pressed;

        // True if a mapped keyboard key is pressed
        bool keyboardKeyPressed { false };

        // If we opened an SDL2 Game controller handle from this class, keep it and inject it into all gamepads we send out
        // Indexed by instanceID
        QMap<int, SDL_GameController *> gameControllerHandles;

        // Remapping stuff

        // If true, do not send any data to children
        bool remapMode{ false };

        // The GUID being remapped
        QString remapModeGUID;

        // The Key (SDL Game controller API button/axis) of the GUID getting the remap
        Key remapModeKey;
        Type remapModeType;

        // If set to true, remain true until ignoreModeButton is released
        bool ignoreMode{ false };

        // The Joystick button/axis/hat pressed/tilted to set the remapping
        Val ignoreModeVal;

        // The GUID that was just remapped
        QString ignoreModeGUID;

        // The instanceID that was just remapped
        int ignoreModeInstanceID{ -1 };

        // Remap data
        // Key: GUID
        // Value: Mapping (SDL Game controller API mapping string, see Key, Val, Mapping up above)
        QMap<QString, Mapping> gameControllerToJoystick;

        // Convert analog stick values to d-pad presses and vice versa
        // This is useful to both LibretroCore and the UI (GlobalGamepad)
        // analogToDpad is always active when not playing
        QMap<QString, bool> analogToDpad;
        QMap<QString, bool> dpadToAnalog;

        bool dpadToAnalogKeyboard { false };

        // Deadzone data
        // Indexed by GUID
        // TODO: Load/store to/from disk
        QMap<QString, QHash<int, Sint16>> deadzones;
        QMap<QString, QHash<int, bool>> deadzoneModes;

        // Have we done initial calibration yet?
        QMap<QString, bool> deadzoneFlag;

        // Keyboard remapping data
        // We keep the keyboard's state here as it comes to us "raw"
        GamepadState keyboardGamepad;

        QMap<int, int> keyboardKeyToSDLButton {
            // Default keyboard mapping
            // US Keyboard
            { Qt::Key_W, SDL_CONTROLLER_BUTTON_DPAD_UP },
            { Qt::Key_A, SDL_CONTROLLER_BUTTON_DPAD_LEFT },
            { Qt::Key_S, SDL_CONTROLLER_BUTTON_DPAD_DOWN },
            { Qt::Key_D, SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
            { Qt::Key_P, SDL_CONTROLLER_BUTTON_A },
            { Qt::Key_L, SDL_CONTROLLER_BUTTON_B },
            { Qt::Key_O, SDL_CONTROLLER_BUTTON_X },
            { Qt::Key_K, SDL_CONTROLLER_BUTTON_Y },
            // Alternate layout
            //{ Qt::Key_L, SDL_CONTROLLER_BUTTON_A },
            //{ Qt::Key_K, SDL_CONTROLLER_BUTTON_B },
            //{ Qt::Key_I, SDL_CONTROLLER_BUTTON_X },
            //{ Qt::Key_J, SDL_CONTROLLER_BUTTON_Y },
            { Qt::Key_Shift, SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
            { Qt::Key_Space, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
            { Qt::Key_Backspace, SDL_CONTROLLER_BUTTON_BACK },
            { Qt::Key_Escape, SDL_CONTROLLER_BUTTON_GUIDE },
            { Qt::Key_Return, SDL_CONTROLLER_BUTTON_START },
            { Qt::Key_1, SDL_CONTROLLER_BUTTON_LEFTSTICK },
            { Qt::Key_3, SDL_CONTROLLER_BUTTON_RIGHTSTICK },
            // Home row
            { Qt::Key_Home, SDL_CONTROLLER_BUTTON_GUIDE },
            // Arrow keys
            { Qt::Key_Up, SDL_CONTROLLER_BUTTON_DPAD_UP },
            { Qt::Key_Down, SDL_CONTROLLER_BUTTON_DPAD_DOWN },
            { Qt::Key_Left, SDL_CONTROLLER_BUTTON_DPAD_LEFT },
            { Qt::Key_Right, SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
            // Number pad
            { Qt::Key_5, SDL_CONTROLLER_BUTTON_A },
            { Qt::Key_4, SDL_CONTROLLER_BUTTON_B },
            { Qt::Key_8, SDL_CONTROLLER_BUTTON_X },
            { Qt::Key_7, SDL_CONTROLLER_BUTTON_Y },
            { Qt::Key_6, SDL_CONTROLLER_BUTTON_BACK },
            { Qt::Key_Enter, SDL_CONTROLLER_BUTTON_START },
        };

        // Internal bookkeeping

        // Count of how many controllers of a certain GUID are connected
        // When 0, the entry may be removed
        // Used to know when it's time to emit controllerConnected()/controllerDisconnected()
        QMap<QString, int> GUIDCount;

        // Helpers

        // If clear is set, analog state is cleared if no d-pad buttons pressed
        GamepadState mapDpadToAnalog( GamepadState gamepad, bool clear = false );
};

QString gameControllerIDToMappingString( int gameControllerID );

int mappingStringToGameControllerID( QString gameControllerString );

// Game controller to Joystick mapping string helpers
Remapper::Key mappingStringToKey( QString keyString , bool *ok = nullptr );
QString keyToMappingString( Remapper::Key key );
QString valToFriendlyString( Remapper::Val val );
QString valToMappingString( Remapper::Val val );
