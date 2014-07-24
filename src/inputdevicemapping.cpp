
#include "libretro.h"

#include "inputdevicemapping.h"


InputDeviceMapping::InputDeviceMapping()
{
    device_type = RETRO_DEVICE_NONE;
}

bool InputDeviceMapping::isValid()
{
    return (device_type != RETRO_DEVICE_NONE);
}
