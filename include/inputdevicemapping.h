
#ifndef INPUTDEVICEMAPPING_H
#define INPUTDEVICEMAPPING_H

#include <unordered_map>

#include <QSettings>
#include <QJSValue>

#include "libretro_types.h"
#include "inputdevice.h"
#include "inputdeviceevent.h"


// Class to map input from some device (eg keyboard, mouse, joystick)
// to retro_device_id events for the given retro_device_type.
class InputDeviceMapping : public QObject
{
    Q_OBJECT

public:
    InputDeviceMapping();
    virtual ~InputDeviceMapping() { }

    virtual bool isValid();

    // takes a QSetting whose current group has been pre-set
    // by the caller to input/portX
    virtual bool populateFromSettings(QSettings &settings);

    // populate a Mapping from a QVariantMap as returned
    // by InputDevice::enumerateDevices
    virtual bool populateFromDict(QVariantMap deviceinfo);

    virtual InputDeviceEvent *eventFromString(QString) = 0;

    retro_device_type deviceType() const { return device_type; }
    void setDeviceType(retro_device_type type) { device_type = type; }


    retro_device_id getMapping(InputDeviceEvent *ev, retro_device_id defaultV = ~0) const
    {
        auto res = mapping.find(ev);
        if (res != mapping.end())
            return res->second;

        return defaultV;
    }

    void setMapping(InputDeviceEvent *ev, retro_device_id id)
    {
        qDebug() << "Previous mapping for " << QString(*ev) << " was: " << mapping[ev];
        mapping[ev] = id;
        qDebug() << "New mapping for " << QString(*ev) << " is: " << id;
    }

    void setMapping(const InputDeviceEvent *ev, retro_device_id id)
    {
        mapping[ev->clone()] = id;
    }

public slots:
    // wait for any input on the underlying device and maps
    // the input id to the given retro_device_id
    virtual QVariant setMappingOnInput(retro_device_id id, QJSValue cb) = 0;
    virtual void cancelMappingOnInput(QVariant cancelInfo) = 0;
    virtual QString getMappingByRetroId(QString retroId);
    virtual void remapMapping(QString previousEvent, QString event, QString retroId);

protected:
    // Type of retro device mapped:
    // JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
    // see libretro.h
    retro_device_type device_type;

    std::unordered_map<InputDeviceEvent *, retro_device_id> mapping;
};

// to be able to use Connection objects in QVariants
Q_DECLARE_METATYPE(QMetaObject::Connection)


#endif
