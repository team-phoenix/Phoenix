
#include "inputdevice.h"


InputDevice::InputDevice() : ids_state()
{
    setType(RETRO_DEVICE_NONE);
}

InputDevice::~InputDevice()
{

}

void InputDevice::setDeviceName(const char *new_name)
{
    device_name = QString(new_name);
}

void InputDevice::setType(retro_device_type new_type)
{
    m_type = new_type;
}
