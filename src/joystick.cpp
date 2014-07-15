#include <QDebug>
#include "joystick.h"

Joystick::Joystick() {
    qDebug() << "new Joystick" << this;

    setType(RETRO_DEVICE_JOYPAD);
    setName("Joystick");
    //axis_count = 0;
    //button_count = 0;
    //hat_count = 0;
    joystick = nullptr;
}

Joystick::~Joystick()
{
    qDebug() << "delete Joystick" << this;
}
