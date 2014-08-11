
#ifndef INPUTDEVICEMAPPING_H
#define INPUTDEVICEMAPPING_H

#include <QSettings>
#include <QMap>

#include "libretro_types.h"
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

    // takes a QSetting whose current group has been pre-set
    // by the caller to input/portX
    virtual bool populateFromSettings(QSettings &settings);

    virtual int32_t eventFromString(QString) = 0;

    retro_device_type deviceType() const { return device_type; }
    void setDeviceType(retro_device_type type) { device_type = type; }

    retro_device_id getMapping(int32_t inputev, retro_device_id defaultVal = ~0) const
    {
        return mapping.value(inputev, defaultVal);
    }
    void setMapping(int32_t inputev, retro_device_id id) { mapping[inputev] = id; }

public slots:
    // wait for any input on the underlying device and maps
    // the input id to the given retro_device_id
    virtual void setMappingOnInput(retro_device_id id) = 0;

protected:
    // Type of retro device mapped:
    // JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
    // see libretro.h
    retro_device_type device_type;

    QMap<int32_t, retro_device_id> mapping;
};


#endif
