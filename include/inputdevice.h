#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QMap>
#include <QString>
#include "libretro.h"

#include "logging.h"


typedef unsigned retro_device_type;


class InputDevice
{
public:
    InputDevice();

    virtual ~InputDevice();

    QString name() const { return m_name; }
    retro_device_type type() const { return m_type; }

    int16_t state(unsigned id) const { return ids_state.value(id, 0); }

protected:
    void setName(const char *new_name);
    void setType(retro_device_type new_type);

    // maps ids to state
    // ids can refer to a button, an axis, etc...
    // depending to the retro_device_type
    QMap<unsigned, int16_t> ids_state;

private:
    // input device name, e.g "Xbox 360 Controller"
    QString m_name;

    // NONE/JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
    retro_device_type m_type;

};

#endif // INPUTDEVICE_H
