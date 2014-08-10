
#include <QSettings>

#include "inputmanager.h"
#include "joystick.h"
#include "keyboard.h"
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
        return new Joystick::Mapping(device);
    } else if (className == "Keyboard") {
        return new Keyboard::Mapping(device);
    } else {
        return nullptr;
    }
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
    devices.insert(0, new Joystick((Joystick::Mapping *)mapping));
}

// load Mapping for a given port from the settings
// TODO: move some of this logic to the InputDeviceMapping class
InputDeviceMapping *InputManager::mappingForPort(unsigned port)
{
    static const QMap<QString, retro_device_id> joypad_buttons {
        { "joypad_b", RETRO_DEVICE_ID_JOYPAD_B },
        { "joypad_y", RETRO_DEVICE_ID_JOYPAD_Y },
        { "joypad_select", RETRO_DEVICE_ID_JOYPAD_SELECT },
        { "joypad_start", RETRO_DEVICE_ID_JOYPAD_START },
        { "joypad_up", RETRO_DEVICE_ID_JOYPAD_UP },
        { "joypad_down", RETRO_DEVICE_ID_JOYPAD_DOWN },
        { "joypad_left", RETRO_DEVICE_ID_JOYPAD_LEFT },
        { "joypad_right", RETRO_DEVICE_ID_JOYPAD_RIGHT },
        { "joypad_a", RETRO_DEVICE_ID_JOYPAD_A },
        { "joypad_x", RETRO_DEVICE_ID_JOYPAD_B },
        { "joypad_l", RETRO_DEVICE_ID_JOYPAD_L },
        { "joypad_r", RETRO_DEVICE_ID_JOYPAD_R },
        { "joypad_l2", RETRO_DEVICE_ID_JOYPAD_L2 },
        { "joypad_r2", RETRO_DEVICE_ID_JOYPAD_R2 },
        { "joypad_l3", RETRO_DEVICE_ID_JOYPAD_L3 },
        { "joypad_r3", RETRO_DEVICE_ID_JOYPAD_R3 },
    };
    QSettings s;
    s.beginGroup("input");
    s.beginGroup(QString("port%1").arg(port));

    // create InputDeviceMapping subclass for the matching driver
    QString input_driver = s.value("input_driver").toString();
    auto *mapping = InputDeviceMappingFactory::createMapping(input_driver);
    if (mapping == nullptr)
        return nullptr;

    // retro device type
    QString device_type = s.value("device_type").toString();
    Q_ASSERT(device_type == "joypad"); // TODO: only supported type for now

    for (auto i = joypad_buttons.constBegin(); i != joypad_buttons.constEnd(); i++) {
        QVariant val = s.value(i.key());
        if (!val.isValid())
            continue;
        QString eventstr = val.toString();

        mapping->setMapping(mapping->eventFromString(eventstr), i.value());
    }
    return mapping;
}
