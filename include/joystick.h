
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <memory>
#include <SDL2/SDL.h>

#include "inputdevice.h"
#include "sdlevents.h"


class Joystick : public InputDevice
{

public:
    Joystick();
    virtual ~Joystick();

    //int axis_count;
    //int button_count;
    //int hat_count;
    //QMap<quint8, Sint16> axes;
    //QMap<unsigned, Uint8> button_states;
    //QMap<quint8, Uint8> hats;

private:
    std::shared_ptr<SDLEvents> events;
    // shared instance between all the class Joystick instances
    static std::weak_ptr<SDLEvents> events_global;

    bool handleSDLEvent(const SDL_Event *event);
    SDLEvents::EventCallback callback;

    SDL_Joystick *joystick;
    SDL_GameController *controller;
    bool device_attached;

    bool deviceAdded(const SDL_Event *event);
    bool deviceRemoved(const SDL_Event *event);

    bool controllerButtonChanged(const SDL_Event *event);
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
