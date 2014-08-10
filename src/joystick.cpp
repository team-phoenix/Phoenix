
#include <functional>
#include <QMap>
#include <QPair>
#include "libretro.h"

#include "joystick.h"
#include "logging.h"


static SDLEvents sdl_events;


Joystick::Joystick(InputDeviceMapping *mapping) : InputDevice(mapping)
{
    setType(RETRO_DEVICE_JOYPAD);

    device_attached = false;
    joystick = nullptr;
    controller = nullptr;

    callback = std::bind(&Joystick::handleSDLEvent, this, std::placeholders::_1);
    sdl_events.registerCallback(&callback);

    // TODO: temporary
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            setDeviceName(SDL_GameControllerName(controller));
            device_attached = true;
            break;
        }
    }
}

Joystick::~Joystick()
{
    sdl_events.removeCallback(&callback);
    if (controller) {
        SDL_GameControllerClose(controller);
    } else if (joystick) {
        SDL_JoystickClose(joystick);
    }
    joystick = nullptr;
    controller = nullptr;
}

// static
QVariantList Joystick::enumerateDevices()
{
    QVariantList list;
    char guidbuf[128];
    int nconnected = SDL_NumJoysticks();

    if (!nconnected) {
        qCCritical(phxInput, "Unable to enumerate joysticks: %s", SDL_GetError());
    }

    for (int i = 0; i < nconnected; i++) {
        const char* jsname = SDL_JoystickNameForIndex(i);
        if (!jsname)
            continue;
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(i), guidbuf, 128);
        list.append(QVariantMap {
            { "text", QString(jsname) }, // for QML model
            { "name", QString(jsname) },
            { "class", "Joystick" },
            { "type", QString(SDL_IsGameController(i) ? "gamecontroller" : "joystick") },
            { "guid", QString(guidbuf) }
        });
    }
    return list;
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
    if (event->type == SDL_CONTROLLERDEVICEREMOVED
            && ControllerMatchEvent(event->cdevice)) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
        device_attached = false;
        qCDebug(phxInput) << "Controller removed";
        return true;
    } else if(JoystickMatchEvent(event->jdevice)) {
        SDL_JoystickClose(joystick);
        joystick = nullptr;
        device_attached = false;
        qCDebug(phxInput) << "Joystick removed";
        return true;
    }
    return false;
}

bool Joystick::controllerButtonChanged(const SDL_Event *event) {
    if (!ControllerMatchEvent(event->cbutton))
        return false;

    const SDL_ControllerButtonEvent *cbutton = &event->cbutton;
    bool is_pressed = (cbutton->type == SDL_CONTROLLERBUTTONDOWN) ? true : false;
    auto retro_id = mapping->getMapping(cbutton->button);
    if (retro_id != (unsigned)~0)
        setState(retro_id, is_pressed);

    return true;
}

bool Joystick::controllerAxisChanged(const SDL_Event *event) {
    if (!ControllerMatchEvent(event->caxis))
        return false;

    const SDL_ControllerAxisEvent *caxis = &event->caxis;
    if (type() == RETRO_DEVICE_JOYPAD) { // make analog stick emulate dpad
        static int16_t threshold = 25000; // arbitrary. TODO: make it configurable
        static const QMap<SDL_GameControllerAxis, QPair<unsigned, unsigned>> mapping {
            { SDL_CONTROLLER_AXIS_LEFTX, { RETRO_DEVICE_ID_JOYPAD_LEFT,
                                           RETRO_DEVICE_ID_JOYPAD_RIGHT } },
            { SDL_CONTROLLER_AXIS_LEFTY, { RETRO_DEVICE_ID_JOYPAD_UP,
                                           RETRO_DEVICE_ID_JOYPAD_DOWN } }
        };
        auto axis = static_cast<SDL_GameControllerAxis>(caxis->axis);
        if (mapping.contains(axis)) {
            auto buttons_id = mapping.value(axis);
            setState(buttons_id.first, (caxis->value < -threshold) ? true : false);
            setState(buttons_id.second, (caxis->value > threshold) ? true : false);
        }
    }
    return true;
}

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
            controllerAxisChanged(event);
            break;

        case SDL_JOYAXISMOTION:
        case SDL_JOYHATMOTION:
            break;
    }
    return false;
}

void Joystick::Mapping::setMappingOnInput(retro_device_id id)
{
}

int32_t Joystick::Mapping::eventFromString(QString evname)
{
    QByteArray _evname = evname.toLatin1();

    auto btn = SDL_GameControllerGetButtonFromString(_evname.constData());
    if (btn != SDL_CONTROLLER_BUTTON_INVALID)
        return static_cast<int32_t>(btn);

    auto axis = SDL_GameControllerGetAxisFromString(_evname.constData());
    if (axis != SDL_CONTROLLER_AXIS_INVALID)
        return static_cast<int32_t>(axis);

    return -1;
}
