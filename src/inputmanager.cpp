
#include "inputmanager.h"


InputManager::InputManager()
{
}

InputManager::~InputManager()
{
    devices.clear(); // XXX: MEMORY LEAK ??
}

void InputManager::append(InputDevice *device)
{
    devices.push_back(device);
}

InputDevice *InputManager::getDevice(unsigned port) const
{
    return devices.at(port);
}

QList<InputDevice *> InputManager::getDevices() const
{
    return devices;
}

void InputManager::scanDevices()
{
    devices.insert(0, new Joystick());
}
