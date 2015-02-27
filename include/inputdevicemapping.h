
#ifndef INPUTDEVICEMAPPING_H
#define INPUTDEVICEMAPPING_H

#include <unordered_map>

#include <QSettings>
#include <QJSValue>

#include "libretro_types.h"
#include "inputdevice.h"
#include "inputdeviceevent.h"
#include "mappingmodel.h"


// Class to map input from some device (eg keyboard, mouse, joystick)
// to retro_device_id events for the given retro_device_type.

typedef QMap<QString, retro_device_id> device_settings_mapping;
typedef std::unordered_map<InputDeviceEvent *, retro_device_id> DeviceMap;

static const device_settings_mapping joypad_settings_mapping {
    { "joypad_b", RETRO_DEVICE_ID_JOYPAD_B },
    { "joypad_y", RETRO_DEVICE_ID_JOYPAD_Y },
    { "joypad_select", RETRO_DEVICE_ID_JOYPAD_SELECT },
    { "joypad_start", RETRO_DEVICE_ID_JOYPAD_START },
    { "joypad_up", RETRO_DEVICE_ID_JOYPAD_UP },
    { "joypad_down", RETRO_DEVICE_ID_JOYPAD_DOWN },
    { "joypad_left", RETRO_DEVICE_ID_JOYPAD_LEFT },
    { "joypad_right", RETRO_DEVICE_ID_JOYPAD_RIGHT },
    { "joypad_a", RETRO_DEVICE_ID_JOYPAD_A },
    { "joypad_x", RETRO_DEVICE_ID_JOYPAD_X },
    { "joypad_l", RETRO_DEVICE_ID_JOYPAD_L },
    { "joypad_r", RETRO_DEVICE_ID_JOYPAD_R },
    { "joypad_l2", RETRO_DEVICE_ID_JOYPAD_L2 },
    { "joypad_r2", RETRO_DEVICE_ID_JOYPAD_R2 },
    { "joypad_l3", RETRO_DEVICE_ID_JOYPAD_L3 },
    { "joypad_r3", RETRO_DEVICE_ID_JOYPAD_R3 },
};

static const QMap<retro_device_id, QString> id_to_qstring {
    { RETRO_DEVICE_ID_JOYPAD_B, "joypad_b" },
    { RETRO_DEVICE_ID_JOYPAD_Y, "joypad_y" },
    { RETRO_DEVICE_ID_JOYPAD_SELECT, "joypad_select" },
    { RETRO_DEVICE_ID_JOYPAD_START, "joypad_start" },
    { RETRO_DEVICE_ID_JOYPAD_UP, "joypad_up" },
    { RETRO_DEVICE_ID_JOYPAD_DOWN, "joypad_down" },
    { RETRO_DEVICE_ID_JOYPAD_LEFT, "joypad_left" },
    { RETRO_DEVICE_ID_JOYPAD_RIGHT, "joypad_right" },
    { RETRO_DEVICE_ID_JOYPAD_A, "joypad_a" },
    { RETRO_DEVICE_ID_JOYPAD_X, "joypad_x" },
    { RETRO_DEVICE_ID_JOYPAD_L, "joypad_l" },
    { RETRO_DEVICE_ID_JOYPAD_R, "joypad_r" },
    { RETRO_DEVICE_ID_JOYPAD_L2, "joypad_l2" },
    { RETRO_DEVICE_ID_JOYPAD_R2, "joypad_r2" },
    { RETRO_DEVICE_ID_JOYPAD_L3, "joypad_l3" },
    { RETRO_DEVICE_ID_JOYPAD_R3, "joypad_r3" },
};

static const QMap<QString, const device_settings_mapping *> settings_mappings {
    { "joypad", &joypad_settings_mapping },
};

class InputDeviceMapping : public QObject {
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

        virtual InputDeviceEvent *eventFromString(QString event) = 0;

        retro_device_type deviceType() const
        {
            return device_type;
        }

        void setDeviceType( retro_device_type type )
        {
            device_type = type;
        }

        retro_device_id getMapping( InputDeviceEvent *ev, retro_device_id defaultV = ~0 ) const
        {
            auto res = mapping.find( ev );

            if( res != mapping.end() ) {
                return res->second;
            }

            return defaultV;
        }

        void setMapping(const InputDeviceEvent *ev, const retro_device_id id, const unsigned port);
        void setMapping(const InputDeviceEvent *ev, retro_device_id id);

        DeviceMap *mappings();

public slots:
        // wait for any input on the underlying device and maps
        // the input id to the given retro_device_id
        virtual QVariant setMappingOnInput( retro_device_id id, QJSValue cb ) = 0;
        virtual void cancelMappingOnInput( QVariant cancelInfo ) = 0;
        virtual QString getMappingByRetroId( QString retroId );
        virtual bool remap(QVariant incoming_event, unsigned retroId, unsigned port);

protected:
        // Type of retro device mapped:
        // JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
        // see libretro.h
        retro_device_type device_type;

signals:

private:
        DeviceMap mapping;

};

// to be able to use Connection objects in QVariants
Q_DECLARE_METATYPE( QMetaObject::Connection )


#endif
