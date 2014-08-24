#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QKeyEvent>

#include "inputdevice.h"
#include "inputdevicemapping.h"


class Keyboard : public InputDevice
{
public:
    Keyboard(InputDeviceMapping *mapping);
    virtual ~Keyboard();

    static QVariantList enumerateDevices();

    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

    class Mapping : public InputDeviceMapping
    {
    public:
        Mapping() {};

        virtual int32_t eventFromString(QString) Q_DECL_OVERRIDE;

    public slots:
        virtual void setMappingOnInput(retro_device_id id) Q_DECL_OVERRIDE {};

    private:
    };

private:
    QWindow *topLevelWindow;

    // process QKeyEvent sent from some widget/window
    // as a button press in this virtual Input Device
    void processKeyEvent(QKeyEvent *event);
};


#endif // KEYBOARD_H
