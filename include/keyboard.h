#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QKeyEvent>

#include "inputdevice.h"
#include "inputdevicemapping.h"


class Keyboard : public InputDevice
{
public:
    Keyboard();
    virtual ~Keyboard();

    static QVariantList enumerateDevices();

    // process QKeyEvent sent from some widget/window
    // as a button press in this virtual Input Device
    void processKeyEvent(QKeyEvent *event);

    class Mapping : public InputDeviceMapping
    {
    public slots:
        virtual void setMappingOnInput(retro_device_id id) Q_DECL_OVERRIDE {};

    private:
    };

};


#endif // KEYBOARD_H
