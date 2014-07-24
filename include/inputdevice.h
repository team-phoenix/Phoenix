#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include "libretro.h"

#include "logging.h"


typedef unsigned retro_device_type;

typedef unsigned retro_device_id;


class InputDevice
{
public:
    InputDevice();

    virtual ~InputDevice();

    QString deviceName() const { return device_name; }
    retro_device_type type() const { return m_type; }

    int16_t state(retro_device_id id) const {
        QMutexLocker lock(&ids_state_mutex);
        return ids_state.value(id, 0);
    }

protected:
    void setDeviceName(const char *new_name);
    void setType(retro_device_type new_type);
    void setState(retro_device_id id, int16_t state) {
        QMutexLocker lock(&ids_state_mutex);
        ids_state[id] = state;
    }

    // maps ids to state
    // ids can refer to a button, an axis, etc...
    // depending to the retro_device_type
    QMap<retro_device_id, int16_t> ids_state;
    mutable QMutex ids_state_mutex;

private:
    // input device name, e.g "Xbox 360 Controller"
    QString device_name;

    // NONE/JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
    retro_device_type m_type;

};

#endif // INPUTDEVICE_H
