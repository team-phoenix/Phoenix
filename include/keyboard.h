#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QKeyEvent>

#include "inputdevice.h"


class Keyboard : public InputDevice
{
public:
    Keyboard();
    virtual ~Keyboard();

    static QVariantList enumerateDevices();

    // process QKeyEvent sent from some widget/window
    // as a button press in this virtual Input Device
    void processKeyEvent(QKeyEvent *event);

};


#endif // KEYBOARD_H
