
#include <QSettings>

#include "inputmanager.h"
#include "joystick.h"
#include "keyboard.h"
#include "inputdevicefactory.h"
#include "inputdevicemappingfactory.h"


InputManager::InputManager()
{
}

InputManager::~InputManager()
{
    devices.clear(); // XXX: MEMORY LEAK ??
}

QVariantList InputManager::enumerateDevices()
{
    QVariantList devices;
    devices += Keyboard::enumerateDevices();
    devices += Joystick::enumerateDevices();
    return devices;
}

InputDeviceMapping *InputManager::mappingForDevice(QVariantMap device)
{
    Q_ASSERT(device.contains("class"));
    auto className = device.value("class").toString();
    if (className == "Joystick") {
//        return new Joystick::Mapping(device);
    } else if (className == "Keyboard") {
//        return new Keyboard::Mapping(device);
    }
    return nullptr;
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
    auto mapping = mappingForPort(0);
    devices.insert(0, InputDeviceFactory::createFromMapping(mapping));
}

// load Mapping for a given port from the settings
// TODO: move some of this logic to the InputDeviceMapping class
InputDeviceMapping *InputManager::mappingForPort(unsigned port)
{
    QSettings s;
    s.beginGroup("input");
    s.beginGroup(QString("port%1").arg(port));

    // create InputDeviceMapping subclass for the matching driver
    QString input_driver = s.value("input_driver").toString();
    auto *mapping = InputDeviceMappingFactory::createMapping(input_driver);
    if (mapping == nullptr)
        return nullptr;

    if (!mapping->populateFromSettings(s)) {
        delete mapping;
        return nullptr;
    }

    return mapping;
}
