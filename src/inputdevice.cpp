
#include "inputdevice.h"


InputDevice::InputDevice(InputDeviceMapping *mapping)
        : m_mapping(mapping),
          ids_state()
{
}

InputDevice::~InputDevice()
{

}

void InputDevice::setDeviceName(const char *new_name)
{
    device_name = QString(new_name);
}
