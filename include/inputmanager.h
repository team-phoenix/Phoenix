
#include <QVector>
#include <QTimer>
#include <QObject>

#include "joystick.h"


class InputManager : public QObject
{
    Q_OBJECT

public:
    InputManager();
    ~InputManager();

    void scanDevices();

    void append(InputDevice *device);

    QList<InputDevice *> getDevices() const;
    InputDevice * getDevice(unsigned port) const;

private:
    QList<InputDevice *> devices;
};
