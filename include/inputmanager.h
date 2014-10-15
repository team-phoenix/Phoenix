
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

public slots:
    QVariantList enumerateDevices();
    // return empty mapping for device
    InputDeviceMapping *mappingForDevice(QVariantMap device);

    // load existing mapping for designated port from settings
    InputDeviceMapping *mappingForPort(unsigned port);

    InputDevice *getDevice(unsigned port) const;

private:
    QList<InputDevice *> devices;
};

#endif
