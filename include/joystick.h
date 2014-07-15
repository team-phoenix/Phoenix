#ifndef JOYINFO_H
#define JOYINFO_H

#include "inputdevice.h"

class Joystick : public InputDevice {

public:

    Joystick();
    ~Joystick();

    //int axis_count;
    //int button_count;
    //int hat_count;
    SDL_Joystick *joystick;
    //QMap<quint8, Sint16> axes;
    //QMap<unsigned, Uint8> button_states;
    //QMap<quint8, Uint8> hats;



};

#endif // JOYINFO_H
