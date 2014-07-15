#include "inputmanager.h"

InputManager::InputManager() {

    connect(&timer, SIGNAL(timeout()), this, SLOT(getDeviceEvents()));

}

InputManager::~InputManager() {
    devices.clear();
    stopTimer();

}

void InputManager::append(InputDevice *device) {

    devices.push_back(device);

}

InputDevice * InputManager::getDevice(unsigned port) {

    current_port = port;

    return devices.at(port);

}

QList<InputDevice *> InputManager::getDevices() {
    return devices;
}

void InputManager::startTimer(int mili_secs) {

    timer.start(mili_secs);

}

void InputManager::stopTimer() {

    timer.stop();


}

// Sets how number of devices to iterate over to find presses
// Set to -1 to process every available joystick
void InputManager::getDeviceEvents() {

    sdl_joystick.onProcessEvent(devices);

}

void InputManager::scanDevices() {

    devices = sdl_joystick.onScan();

}
