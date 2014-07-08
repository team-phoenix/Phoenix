#include <QDebug>
#include "joystick.h"

Joystick::Joystick()
{
    qDebug() << "new Joystick" << this;

    index = -1;
    name = "";
    numAxes = 0;
    numButtons = 0;
    numHats = 0;
    joy = NULL;
    axes.clear();
    buttons.clear();
    hats.clear();
}

Joystick::~Joystick()
{
    qDebug() << "delete Joystick" << this;
}
