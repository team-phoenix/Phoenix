#include "inputdevice.h"

InputDevice::InputDevice() {
    type = 0;
    button_states.clear();
    count = 0;
    name = "";    

}

InputDevice::~InputDevice() {

}

void InputDevice::setCount(int new_count) {
    count = new_count;
}

void InputDevice::setIndex(int new_index) {
    index = new_index;
}

void InputDevice::setName(const char *new_name) {
    name = QString(new_name);
}

void InputDevice::setType(unsigned new_type) {
    type = new_type;
}
