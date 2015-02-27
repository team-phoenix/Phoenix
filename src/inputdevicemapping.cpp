
#include "libretro.h"
#include "inputdevicemapping.h"

#include <QRegExp>


InputDeviceMapping::InputDeviceMapping()
{
    device_type = RETRO_DEVICE_NONE;

}

bool InputDeviceMapping::isValid() {
    return ( device_type != RETRO_DEVICE_NONE );
}

bool InputDeviceMapping::populateFromSettings( QSettings &s )
{
    // retro device type
    QString device_type = s.value( "device_type" ).toString();

    if( device_type != "joypad" ) {
        // TODO: only supported type for now
        return false;
    }

    setDeviceType( RETRO_DEVICE_JOYPAD );

    auto *map = settings_mappings.value( device_type );

    for(auto i = map->constBegin(); i != map->constEnd(); i++) {
        QVariant val = s.value(i.key());

        if( !val.isValid()) {
            continue;
        }

        QString eventstr = val.toString();

        InputDeviceEvent *ev = eventFromString(eventstr);

        if(ev != nullptr) {
            setMapping(ev, i.value());
        }
    }

    return true;
}

bool InputDeviceMapping::populateFromDict(QVariantMap deviceinfo)
{
    Q_UNUSED( deviceinfo );
    setDeviceType( RETRO_DEVICE_JOYPAD ); // TODO
    return true;
}

QString InputDeviceMapping::getMappingByRetroId(QString retroId)
{
    for( auto m = mapping.begin(); m != mapping.end(); ++m ) {
        if( m->second == retroId.toUInt() ) {
            return QString( *m->first );
        }
    }

    return "None";
}

bool InputDeviceMapping::remap(QVariant incoming_event, unsigned retroId, unsigned port)
{
    // Remove the previous mapping

    InputDeviceEvent *new_event = incoming_event.value<InputDeviceEvent *>();

    if (!new_event)
        return false;


    for( auto m = mapping.begin(); m != mapping.end(); ++m) {
        auto iter_id = m->second;
        auto iter_event = m->first;

        if (retroId == iter_id) {
            if (iter_event == new_event)
                return true;
            mapping.erase(iter_event);
        }
    }

    setMapping(new_event, retroId, port);

    return true;
}

DeviceMap *InputDeviceMapping::mappings()
{
    return &mapping;
}

void InputDeviceMapping::setMapping(const InputDeviceEvent *ev, retro_device_id id)
{
    mapping[ev->clone()] = id;
}


void InputDeviceMapping::setMapping(const InputDeviceEvent *ev, const retro_device_id id, const unsigned port )
{
    mapping[ev->clone()] = id;

    //delete ev;

    // Time to save to settings;
    QSettings s;
    s.beginGroup("input");
    s.beginGroup(QString("port%1").arg(port));
    s.setValue( id_to_qstring.value(id), QString(*ev));

}
