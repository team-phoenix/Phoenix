#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <memory>

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
        virtual QVariant setMappingOnInput(retro_device_id id, QJSValue cb) Q_DECL_OVERRIDE;
        virtual void cancelMappingOnInput(QVariant cancelInfo) Q_DECL_OVERRIDE;

    private:
        // only used by setMappingOnInput helper function
        std::unique_ptr<Keyboard> keyboard;
    };

private:
    QWindow *topLevelWindow;

    // process QKeyEvent sent from some widget/window
    // as a button press in this virtual Input Device
    void processKeyEvent(QKeyEvent *event);
};


#endif // KEYBOARD_H
