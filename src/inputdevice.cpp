
#include "inputdevice.h"
#include "inputdevicemapping.h"


InputDevice::InputDevice( InputDeviceMapping *mapping )
    : m_mapping( mapping ),
      ids_state() {
}

InputDevice::~InputDevice() {
    delete m_mapping;
}

void InputDevice::setDeviceName( const char *new_name ) {
    device_name = QString( new_name );
}
