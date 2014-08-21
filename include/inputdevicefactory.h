
#ifndef INPUTDEVICE_FACTORY_H
#define INPUTDEVICE_FACTORY_H

#include <typeinfo>
#include "inputdevice.h"
#include "inputdevicemapping.h"
#include "joystick.h"
#include "keyboard.h"


class InputDeviceFactory
{
public:
    static InputDevice *createFromMapping(InputDeviceMapping *mapping)
    {
        auto &type = typeid(*mapping);

        if (type == typeid(Keyboard::Mapping))
            return new Keyboard(mapping);
        else if (type == typeid(Joystick::Mapping))
            return new Joystick(mapping);

        return nullptr;
    }
};

#endif
