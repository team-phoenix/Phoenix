
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

    void handleSDLEvent(const SDL_Event *event);
    SDLEvents::EventCallback callback;
    bool device_attached;

    void deviceAdded(const SDL_Event *event);
    void deviceRemoved(const SDL_Event *event);

    void controllerButtonChanged(const SDL_Event *event);
    void joyButtonChanged(const SDL_Event *event);

    SDL_Joystick *joystick;
    SDL_GameController *controller;
};

#endif // JOYSTICK_H
