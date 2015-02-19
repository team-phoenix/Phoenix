
#include "libretro.h"
#include "inputdevicemapping.h"

#include <QRegExp>


InputDeviceMapping::InputDeviceMapping() {
    device_type = RETRO_DEVICE_NONE;
}

bool InputDeviceMapping::isValid() {
    return ( device_type != RETRO_DEVICE_NONE );
}

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
    value = value.remove("joypad_", Qt::CaseInsensitive);
    return value.left(1).toUpper() + value.mid(1);

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


void InputDeviceMapping::setMapping( InputDeviceEvent *ev, retro_device_id id, unsigned port ) {
    //qDebug() << "Previous mapping for " << QString( *ev ) << " was: " << mapping[ev];

    mapping[ev] = id;
    Q_UNUSED(port)
    //qDebug() << "New mapping for " << QString( *ev ) << " is: " << mapping[ev];

    //QSettings s;
    //s.beginGroup( "input" );
    //s.beginGroup( QString( "port%1" ).arg( port ) );
    //s.setValue( id_to_qstring.value( id ), QString( *ev ) );
}
