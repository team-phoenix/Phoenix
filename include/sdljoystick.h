#ifndef INPUTSDLJoystick_H
#define INPUTSDLJoystick_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QListIterator>
#include "joystick.h"
#include "logging.h"
#include "libretro.h"

class SDLJoystick : public QObject
{
    Q_OBJECT

    QTimer joystickTimer;

public:
    QList<Joystick *> joys;

    explicit SDLJoystick(QObject *parent = 0);
    ~SDLJoystick();
    void convertToRetroDevice(unsigned &id, int button);
    void setDeadZone(Uint16 &axis_value);

signals:
    void joysChanged(int count, QListIterator<Joystick *> i);
    void dataChanged(bool is_pressed, unsigned port, unsigned device, unsigned index, unsigned id);
    void dataChanged(unsigned port, unsigned device, unsigned index, unsigned id);

public slots:
    void onScan();
    void onStart(int eventTimeout = 20);
    void onStop();
    void onProcessEvent();

};

#endif // INPUTSDLJoystick_H
