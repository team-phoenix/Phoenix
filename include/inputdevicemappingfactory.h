
#ifndef INPUTDEVICEMAPPING_FACTORY_H
#define INPUTDEVICEMAPPING_FACTORY_H

class InputDeviceMappingFactory
{
public:
    static InputDeviceMapping *createMapping(QString type)
    {
        if (type == "sdl_joystick" || type == "sdl_gamecontroller")
            return new Joystick::Mapping();
        else if (type == "qt_keyboard")
            return new Keyboard::Mapping();
        else
            return nullptr;
    }

};

#endif
