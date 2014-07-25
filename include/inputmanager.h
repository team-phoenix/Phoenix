
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QList>
#include <QObject>

#include "inputdevice.h"


class InputManager : public QObject
{
    Q_OBJECT

public:
    InputManager();
    ~InputManager();

    void scanDevices();

    void append(InputDevice *device);

    QList<InputDevice *> getDevices() const;
    InputDevice *getDevice(unsigned port) const;

public slots:
    QVariantList enumerateDevices();

private:
    QList<InputDevice *> devices;
};

#endif
