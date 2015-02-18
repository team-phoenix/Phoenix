
#include "libretro.h"

#include "inputdevicemapping.h"


InputDeviceMapping::InputDeviceMapping() {
    device_type = RETRO_DEVICE_NONE;
}

QList<QObject *> InputDeviceMapping::model()
{
    updateModel();
    return m_model;
}

void InputDeviceMapping::updateModel()
{
    QList<QObject *> list;
    for( auto m = mapping.begin(); m != mapping.end(); ++m ) {
        QString event = QString( *( m->first ) );
        QString id = QString::number(m->second);
        list.append( new MappingModel(event, id));

        //qDebug() <<  prev_event << " == " << QString(previousEvent) << " :: " << "ev:  " << QString(*ev);
    }

    m_model = list;
}

void InputDeviceMapping::deleteModel()
{
    for (auto obj : model())
        delete obj;
}

bool InputDeviceMapping::isValid() {
    return ( device_type != RETRO_DEVICE_NONE );
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

bool InputDeviceMapping::populateFromSettings( QSettings &s ) {
    // retro device type
    QString device_type = s.value( "device_type" ).toString();

    if( device_type != "joypad" ) {
        // TODO: only supported type for now
        return false;
    }

    setDeviceType( RETRO_DEVICE_JOYPAD );

    auto *mapping = settings_mappings.value( device_type );

    for( auto i = mapping->constBegin(); i != mapping->constEnd(); i++ ) {
        QVariant val = s.value( i.key() );

        if( !val.isValid() ) {
            continue;
        }

        QString eventstr = val.toString();

        InputDeviceEvent *ev = eventFromString( eventstr );

        if( ev != nullptr ) {
            setMapping( ev, i.value() );
        }
    }

    return true;
}

bool InputDeviceMapping::populateFromDict( QVariantMap deviceinfo ) {
    Q_UNUSED( deviceinfo );
    setDeviceType( RETRO_DEVICE_JOYPAD ); // TODO
    return true;
}

QString InputDeviceMapping::getMappingByRetroId( QString retroId ) {
    for( auto m = mapping.begin(); m != mapping.end(); ++m ) {
        if( m->second == retroId.toUInt() ) {
            // SIGILL IS HERE
            return QString( *m->first );
        }
    }

    return "None";
}

QString InputDeviceMapping::getGamepadName(QString retroID)
{
    //"joypad_"
    QString value = id_to_qstring.value(retroID.toUInt(), "");
    value = value.remove("joypad_");
    return value.left(1).toUpper() + value.mid(1);

}

bool InputDeviceMapping::collisionDetected(QString event, unsigned index)
{
    int i = 0;
    bool result = false;
    for (auto *obj_ptr : model()) {
        auto mapping_obj = static_cast<MappingModel *>(obj_ptr);
        result = mapping_obj->deviceEvent() == event;
        //qDebug() << mapping_obj->deviceEvent() << " " <<  i << " index: " << index << " " << event;

        if (result) {
            if (i == index) {
                result = false;
            }
            break;
        }
        ++i;
    }

    return result;
}

void InputDeviceMapping::remapMapping( QString previousEvent, QVariant event, QString retroId, unsigned port ) {
    // Remove the previous mapping
    auto prevEv = eventFromString( previousEvent );

    if( prevEv != nullptr ) {
        mapping.erase( prevEv );
    }

    InputDeviceEvent *ev = event.value<InputDeviceEvent *>();

    if( ev ) {
        for( auto m = mapping.begin(); m != mapping.end(); ++m ) {
            //qDebug() << QString( *( m->first ) );
            QString prev_event = QString( *( m->first ) );

            //qDebug() <<  prev_event << " == " << QString(previousEvent) << " :: " << "ev:  " << QString(*ev);

            //if( prev_event == QString( *ev ) ) {
             //   qDebug() << "Value is already mapped";
               // return;
            //}


        }

        setMapping( ev, retroId.toUInt(), port );
    }

}

void InputDeviceMapping::saveModel(unsigned port)
{
    QSettings settings;
    settings.beginGroup("input");
    settings.beginGroup( QString( "port%1" ).arg( port ) );
    for (QObject *obj : model()) {
        auto mapping_obj = static_cast<MappingModel *>(obj);
        settings.setValue( id_to_qstring.value(mapping_obj->retroID().toUInt()), mapping_obj->deviceEvent());

    }
}

void InputDeviceMapping::setMapping( InputDeviceEvent *ev, retro_device_id id, unsigned port ) {
    //qDebug() << "Previous mapping for " << QString( *ev ) << " was: " << mapping[ev];

    mapping[ev] = id;
    //qDebug() << "New mapping for " << QString( *ev ) << " is: " << mapping[ev];

    //QSettings s;
    //s.beginGroup( "input" );
    //s.beginGroup( QString( "port%1" ).arg( port ) );
    //s.setValue( id_to_qstring.value( id ), QString( *ev ) );
}
