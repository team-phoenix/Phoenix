
#include <QGuiApplication>
#include <QWindow>

#include "keyboard.h"
#include "logging.h"
#include "keyboardevents.h"


Keyboard::Keyboard( InputDeviceMapping *mapping ) : InputDevice( mapping ) {
    setDeviceName( "Keyboard (Qt KeyEvent)" );

}

Keyboard::~Keyboard() {

}

QVariantList Keyboard::enumerateDevices() {
    return {
        QVariantMap {
            {"text", "Keyboard"}, // for QML model
            {"name", "Keyboard"},
            {"driver", "qt_keyboard"},
            {"type", "Keyboard"},
        }
    };
}

bool Keyboard::eventFilter( QObject *obj, QEvent *event ) {
    Q_UNUSED( obj );
    auto type = event->type();

    if( type == QEvent::KeyPress || type == QEvent::KeyRelease ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );
        processKeyEvent( keyEvent );
        return true;
    }

    return false;
}

inline void Keyboard::processKeyEvent( QKeyEvent *event ) {
    bool is_pressed = ( event->type() == QEvent::KeyPress ) ? true : false;
    auto ev = KeyboardKeyEvent::fromKeyEvent( event );
    emit inputEventReceived( new KeyboardKeyEvent(ev), is_pressed );

    auto retro_id = m_mapping->getMapping( &ev );

    if( retro_id != ( unsigned )~0 ) {
        setState( retro_id, is_pressed );
    }
}

QVariant Keyboard::Mapping::setMappingOnInput( retro_device_id id, QJSValue cb ) {
    auto conn = std::make_shared<QMetaObject::Connection>();

    auto handleInput = [this, id, cb, conn]( InputDeviceEvent *ev, int16_t val ) mutable {
        if( val == 0 ) {
            return;
        }

        this->setMapping( ev, id );
        //        if (cb.isCallable())
        //            cb.call({ id, ev });
        QObject::disconnect( *conn );
    };

    if( !keyboard ) {
        // no std::make_unique in c++11
        keyboard = std::unique_ptr<Keyboard>( new Keyboard( this ) );
    }

    *conn = connect( keyboard.get(), &InputDevice::inputEventReceived, handleInput );
    qCDebug( phxInput ) << "waiting for keyboard input";
    return QVariant::fromValue( *conn );
}

void Keyboard::Mapping::cancelMappingOnInput( QVariant cancelInfo ) {
    if( cancelInfo.canConvert<QMetaObject::Connection>() ) {
        QObject::disconnect( cancelInfo.value<QMetaObject::Connection>() );
    }
}

InputDeviceEvent *Keyboard::Mapping::eventFromString( QString evname ) {
    auto ev = KeyboardKeyEvent::fromString( evname );

    if( ev.isValid() ) {
        return ev.clone();
    }

    return nullptr;
}
