
#include "libretro.h"

#include "inputdevicemapping.h"


InputDeviceMapping::InputDeviceMapping()
{
    device_type = RETRO_DEVICE_NONE;
}

bool InputDeviceMapping::isValid()
{
    return (device_type != RETRO_DEVICE_NONE);
}

typedef QMap<QString, retro_device_id> device_settings_mapping;

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
    { "joypad_x", RETRO_DEVICE_ID_JOYPAD_B },
    { "joypad_l", RETRO_DEVICE_ID_JOYPAD_L },
    { "joypad_r", RETRO_DEVICE_ID_JOYPAD_R },
    { "joypad_l2", RETRO_DEVICE_ID_JOYPAD_L2 },
    { "joypad_r2", RETRO_DEVICE_ID_JOYPAD_R2 },
    { "joypad_l3", RETRO_DEVICE_ID_JOYPAD_L3 },
    { "joypad_r3", RETRO_DEVICE_ID_JOYPAD_R3 },
};

static const QMap<QString, const device_settings_mapping *> settings_mappings {
    { "joypad", &joypad_settings_mapping },
};

bool InputDeviceMapping::populateFromSettings(QSettings &s)
{
    // retro device type
    QString device_type = s.value("device_type").toString();
    if (device_type != "joypad") {
        // TODO: only supported type for now
        return false;
    }
    setDeviceType(RETRO_DEVICE_JOYPAD);

    auto *mapping = settings_mappings.value(device_type);
    for (auto i = mapping->constBegin(); i != mapping->constEnd(); i++) {
        QVariant val = s.value(i.key());
        if (!val.isValid())
            continue;
        QString eventstr = val.toString();

        InputDeviceEvent *ev = eventFromString(eventstr);
        if (ev != nullptr)
            setMapping(ev, i.value());
    }

    return true;
}

bool InputDeviceMapping::populateFromDict(QVariantMap deviceinfo)
{
    setDeviceType(RETRO_DEVICE_JOYPAD); // TODO
    return true;
}
