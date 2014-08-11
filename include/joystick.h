
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <memory>
#include <SDL2/SDL.h>

#include "inputdevice.h"
#include "inputdevicemapping.h"
#include "sdlevents.h"


class Joystick : public InputDevice
{
public:
    Joystick(InputDeviceMapping *mapping);
    virtual ~Joystick();

    // enumerate plugged-in devices
    static QVariantList enumerateDevices();

    class Mapping : public InputDeviceMapping
    {
    public:
        Mapping() {};

        virtual int32_t eventFromString(QString) Q_DECL_OVERRIDE;

    public slots:
        virtual void setMappingOnInput(retro_device_id id) Q_DECL_OVERRIDE;

    private:
    };

private:
    std::shared_ptr<SDLEvents> events;

    Mapping *m_mapping;

    bool handleSDLEvent(const SDL_Event *event);
    SDLEvents::EventCallback callback;

    SDL_Joystick *joystick;
    SDL_GameController *controller;
    bool device_attached;

    bool deviceAdded(const SDL_Event *event);
    bool deviceRemoved(const SDL_Event *event);

    bool controllerButtonChanged(const SDL_Event *event);
    bool controllerAxisChanged(const SDL_Event *event);

    bool joyButtonChanged(const SDL_Event *event);

    // convenience functions to check that an event matches the current joystick/controller
    template <typename SDLEventType> bool ControllerMatchEvent(const SDLEventType &event);
    template <typename SDLEventType> bool JoystickMatchEvent(const SDLEventType &event);
};

template <typename SDLEventType>
inline bool Joystick::ControllerMatchEvent(const SDLEventType &event)
{
    auto joyid = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
    return controller != nullptr && event.which == joyid;
}

template <typename SDLEventType>
inline bool Joystick::JoystickMatchEvent(const SDLEventType &event)
{
    return joystick != nullptr && event.which == SDL_JoystickInstanceID(joystick);
}

#endif // JOYSTICK_H
