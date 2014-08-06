
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QList>
#include <QObject>

#include "inputdevice.h"
#include "inputdevicemapping.h"


class InputManager : public QObject
{
    Q_OBJECT

public:
    InputManager();
    virtual ~InputManager();

    void scanDevices();

    void append(InputDevice *device);

    QList<InputDevice *> getDevices() const;
    InputDevice *getDevice(unsigned port) const;

public slots:
    QVariantList enumerateDevices();
    InputDeviceMapping *mappingForDevice(QVariantMap device);

private:
    QList<InputDevice *> devices;
};

#endif
