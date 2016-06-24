#include "SDLunloader.h"

SDLUnloader::SDLUnloader() {

}

void SDLUnloader::commandIn( Command command, QVariant data, qint64 timeStamp ) {
    emit commandOut( command, data, timeStamp );

    switch( command ) {
        case Command::RemoveController: {
            GamepadState gamepad = data.value<GamepadState>();

            int joystickID = gamepad.joystickID;
            SDL_GameController *gamecontrollerHandle = gamepad.gamecontrollerHandle;
            SDL_Joystick *joystickHandle = gamepad.joystickHandle;

            // Shut down controller-related SDL structures
            {
                if( gamepad.haptic ) {
                    SDL_HapticStopAll( gamepad.haptic );
                    SDL_HapticClose( gamepad.haptic );
                }

                if( SDL_IsGameController( joystickID ) ) {
                    SDL_GameControllerClose( gamecontrollerHandle );
                } else {
                    SDL_JoystickClose( joystickHandle );
                }
            }

            qDebug() << "Removed controller, joystickID:" << joystickID << "instanceID:"
                     << gamepad.instanceID << "Number of gamepads (SDL):" << SDL_NumJoysticks();

            break;
        }

        default:
            break;
    }
}
