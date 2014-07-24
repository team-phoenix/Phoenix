
#ifndef INPUTDEVICEMAPPING_H
#define INPUTDEVICEMAPPING_H

#include <QMap>

#include "inputdevice.h"


// Class to map input from some device (eg keyboard, mouse, joystick)
// to retro_device_id events for the given retro_device_type.
class InputDeviceMapping : public QObject
{
    Q_OBJECT

public:
    InputDeviceMapping();
    virtual ~InputDeviceMapping() { };

    virtual bool isValid();

    retro_device_type deviceType() const { return device_type; }
    void setDeviceType(retro_device_type type) { device_type = type; }

    retro_device_id getMapping(int32_t inputev) const;
    void setMapping(int32_t inputev, retro_device_id id);

public slots:

protected:
    // Type of retro device mapped:
    // JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
    // see libretro.h
    retro_device_type device_type;

    QMap<int32_t, retro_device_id> mapping;
};


#endif
