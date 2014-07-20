
#include <functional>
#include <QHash>
#include "libretro.h"

#include "joystick.h"
#include "logging.h"


std::weak_ptr<SDLEvents> Joystick::events_global;

Joystick::Joystick()
{
    setType(RETRO_DEVICE_JOYPAD);

    // Create a shared SDLEvents instance on-demand when a Joystick instance is
    // created. That way, when no joystick are used, SDL won't be initialized.
    if (events_global.expired()) {
        events = std::make_shared<SDLEvents>();
        events_global = events;
    } else {
        events = events_global.lock();
    }

    callback = std::bind(&Joystick::handleSDLEvent, this, std::placeholders::_1);
    events->registerCallback(&callback);

    device_attached = false;
    joystick = nullptr;
    controller = nullptr;
}

Joystick::~Joystick()
{
    events->removeCallback(&callback);
    if (controller) {
        SDL_GameControllerClose(controller);
    } else if (joystick) {
        SDL_JoystickClose(joystick);
    }
    joystick = nullptr;
    controller = nullptr;
}

bool Joystick::deviceAdded(const SDL_Event *event)
{
    // TODO: check that device matches the requested one
    //if(SDL_JoystickGetDeviceGUID() == ...
    if (event->type == SDL_CONTROLLERDEVICEADDED) {
        controller = SDL_GameControllerOpen(event->cdevice.which);
        setDeviceName(SDL_GameControllerName(controller));
    } else {
        if (SDL_IsGameController(event->jdevice.which)) {
            // joystick is a supported game controller
            // let's wait for the CONTROLLERADDED event to add it
            return false;
        }
        joystick = SDL_JoystickOpen(event->jdevice.which);
        setDeviceName(SDL_JoystickName(joystick));
    }
    device_attached = true;
    return true;
}

bool Joystick::deviceRemoved(const SDL_Event *event)
{
    if (event->type == SDL_CONTROLLERDEVICEREMOVED && ControllerMatchEvent(event)) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
        device_attached = false;
        qCDebug(phxInput) << "Controller removed";
        return true;
    } else if(JoystickMatchEvent(event)) {
        SDL_JoystickClose(joystick);
        joystick = nullptr;
        device_attached = false;
        qCDebug(phxInput) << "Joystick removed";
        return true;
    }
    return false;
}

bool Joystick::controllerButtonChanged(const SDL_Event *event) {
    static const QHash<SDL_GameControllerButton, unsigned> mapping {
        { SDL_CONTROLLER_BUTTON_A, RETRO_DEVICE_ID_JOYPAD_A },
        { SDL_CONTROLLER_BUTTON_B, RETRO_DEVICE_ID_JOYPAD_B },
        { SDL_CONTROLLER_BUTTON_X, RETRO_DEVICE_ID_JOYPAD_X },
        { SDL_CONTROLLER_BUTTON_Y, RETRO_DEVICE_ID_JOYPAD_Y },
        { SDL_CONTROLLER_BUTTON_BACK, RETRO_DEVICE_ID_JOYPAD_SELECT },
        { SDL_CONTROLLER_BUTTON_START, RETRO_DEVICE_ID_JOYPAD_START },
        { SDL_CONTROLLER_BUTTON_LEFTSHOULDER, RETRO_DEVICE_ID_JOYPAD_L },
        { SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, RETRO_DEVICE_ID_JOYPAD_R },
        { SDL_CONTROLLER_BUTTON_DPAD_UP, RETRO_DEVICE_ID_JOYPAD_UP },
        { SDL_CONTROLLER_BUTTON_DPAD_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN },
        { SDL_CONTROLLER_BUTTON_DPAD_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT },
        { SDL_CONTROLLER_BUTTON_DPAD_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT }
    };
    if (!ControllerMatchEvent(event))
        return false;

    const SDL_ControllerButtonEvent *cbutton = &event->cbutton;
    bool is_pressed = (cbutton->type == SDL_CONTROLLERBUTTONDOWN) ? true : false;
    auto retro_id = mapping.value(static_cast<SDL_GameControllerButton>(cbutton->button), (unsigned)~0);
    if (retro_id != (unsigned)~0)
        setState(retro_id, is_pressed);

    return true;
}

/*void Joystick::axisChanged(const SDL_Event *event) {

}*/

bool Joystick::handleSDLEvent(const SDL_Event *event)
{
    switch (event->type) {
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_JOYDEVICEADDED:
            if (!device_attached)
                return deviceAdded(event);
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
        case SDL_JOYDEVICEREMOVED:
            if (device_attached)
                return deviceRemoved(event);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            controllerButtonChanged(event);
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
//            joyButtonChanged(event);
            break;

        case SDL_CONTROLLERAXISMOTION:
        case SDL_JOYAXISMOTION:
        case SDL_JOYHATMOTION:
//            OnControllerAxis(event);
            break;
    }
    return false;
}
