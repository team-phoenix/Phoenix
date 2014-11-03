
#include <QSettings>
#include <QQmlEngine>
#include <QGuiApplication>

#include "inputmanager.h"
#include "joystick.h"
#include "keyboard.h"
#include "inputdevicefactory.h"
#include "inputdevicemappingfactory.h"


InputManager::InputManager()
{
    top_window = nullptr;
    settings_window = nullptr;
}

InputManager::~InputManager()
{
    for (auto& device : devices)
        delete device;
    devices.clear();
}

QVariantList InputManager::enumerateDevices()
{
    QVariantList devices;
    devices.append(Keyboard::enumerateDevices());
    devices.append(Joystick::enumerateDevices());
    return devices;
}

InputDeviceMapping *InputManager::mappingForDevice(QVariantMap device)
{
    Q_ASSERT(device.contains("driver"));
    QString driverName = device.value("driver").toString();
    return InputDeviceMappingFactory::createMapping(driverName);
}

void InputManager::append(InputDevice *device)
{
    devices.push_back(device);
}

InputDevice *InputManager::getDevice(unsigned port) const
{
    //if (port >= devices.length()) {
     //   qCDebug(phxLibrary) << "Input device isnt connected: "  << devices.length();
    //    return nullptr;
    //}
    InputDevice* device = devices.at(port);
    // Don't allow QML to take ownership of our devices
    QQmlEngine::setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QList<InputDevice *> InputManager::getDevices() const
{
    return devices;
}

void InputManager::scanDevices()
{
    auto *mapping = mappingForPort(0);
    if (mapping == nullptr) {
        // define default input mapping
        // TODO: move this to a separate func
        QSettings s;
        s.beginGroup("input");
        s.beginGroup("port0");
        s.setValue("input_driver", "qt_keyboard");
        s.setValue("device_type", "joypad");
        s.setValue("joypad_select", "Backspace");
        s.setValue("joypad_start", "Return");
        s.setValue("joypad_a", "e");
        s.setValue("joypad_b", "r");
        s.setValue("joypad_x", "d");
        s.setValue("joypad_y", "f");
        s.setValue("joypad_up", "Up");
        s.setValue("joypad_down", "Down");
        s.setValue("joypad_left", "Left");
        s.setValue("joypad_right", "Right");
        s.setValue("joypad_l", "t");
        s.setValue("joypad_r", "g");
        mapping = mappingForPort(0);

    }

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

void InputManager::attachDevices()
{
    for (int i=0; i < devices.length(); ++i) {
        InputDevice *device = devices.at(i);\
        if (device == nullptr)
            break;
        top_window = QGuiApplication::topLevelWindows()[0];
        top_window->installEventFilter(device);
        settings_window = QGuiApplication::allWindows()[0];
        settings_window->installEventFilter(device);
    }

}

void InputManager::removeDevices()
{
    for (int i=0; i < devices.length(); ++i) {
        InputDevice *device = devices.at(i);
        if (device == nullptr)
            break;

        top_window->removeEventFilter(device);
        settings_window->removeEventFilter(device);

    }
}
