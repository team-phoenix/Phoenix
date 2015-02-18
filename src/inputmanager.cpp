
#include <QSettings>
#include <QQmlEngine>
#include <QtConcurrent>
#include <QGuiApplication>

#include "inputmanager.h"
#include "joystick.h"
#include "keyboard.h"
#include "inputdevicefactory.h"
#include "inputdevicemappingfactory.h"


InputManager::InputManager() {
    top_window = nullptr;
    settings_window = nullptr;
}

InputManager::~InputManager() {
    for( auto &device : devices ) {
        delete device;
    }

    devices.clear();
}

QVariantList InputManager::enumerateDevices() {
    QVariantList devices;
    devices.append( Keyboard::enumerateDevices() );
    devices.append( Joystick::enumerateDevices() );
    return devices;
}

InputDeviceMapping *InputManager::mappingForDevice( QVariantMap device ) {
    Q_ASSERT( device.contains( "driver" ) );
    QString driverName = device.value( "driver" ).toString();
    return InputDeviceMappingFactory::createMapping( driverName );
}

void InputManager::append( InputDevice *device ) {
    devices.push_back( device );
}

InputDevice *InputManager::getDevice( unsigned port ) const {
    //if (port >= devices.length()) {
    //   qCDebug(phxLibrary) << "Input device isnt connected: "  << devices.length();
    //    return nullptr;
    //}
    InputDevice *device = devices.at( port );
    // Don't allow QML to take ownership of our devices
    QQmlEngine::setObjectOwnership( device, QQmlEngine::CppOwnership );
    return device;
}

QList<InputDevice *> InputManager::getDevices() const {
    return devices;
}

void InputManager::scanDevicesAsync() {
    QFuture<void> fut = QtConcurrent::run( this, &InputManager::scanDevices );
    Q_UNUSED( fut )
}

void InputManager::scanKeyboard() {
    int current_port = devices.length();
    auto *keyboard_mapping = mappingForPort( current_port );

    if( keyboard_mapping == nullptr ) {
        // define default input mapping
        // TODO: move this to a separate func
        QSettings s;
        s.beginGroup( "input" );
        s.beginGroup( QString( "port%1" ).arg( current_port ) );
        s.setValue( "input_driver", "qt_keyboard" );
        s.setValue( "device_type", "joypad" );
        s.setValue( "joypad_select", "Backspace" );
        s.setValue( "joypad_start", "Return" );
        s.setValue( "joypad_a", "e" );
        s.setValue( "joypad_b", "r" );
        s.setValue( "joypad_x", "d" );
        s.setValue( "joypad_y", "f" );
        s.setValue( "joypad_up", "Up" );
        s.setValue( "joypad_down", "Down" );
        s.setValue( "joypad_left", "Left" );
        s.setValue( "joypad_right", "Right" );
        s.setValue( "joypad_l", "t" );
        s.setValue( "joypad_r", "g" );


        s.setValue("joypad_leftstick", "");
        s.setValue("joypad_rightstick", "");

        keyboard_mapping = mappingForPort( current_port );

    }

    devices.insert( current_port, InputDeviceFactory::createFromMapping( keyboard_mapping ) );
}

void InputManager::scanJoysticks() {
    qCDebug( phxInput ) << "Started controller scan.";

    int current_port = devices.length();
    int joysticks = SDL_NumJoysticks();

    for( int i = 0; i < joysticks; ++i ) {
        auto *sdl_mapping = mappingForPort( current_port );

        if( sdl_mapping == nullptr ) {
            QSettings s;
            s.beginGroup( "input" );
            s.beginGroup( QString( "port%1" ).arg( current_port ) );
            s.setValue( "input_driver", "sdl_joystick" );
            s.setValue( "device_type", "joypad" );

            s.setValue( "joypad_a", "a" );
            s.setValue( "joypad_b", "b" );
            s.setValue( "joypad_y", "y" );
            s.setValue( "joypad_x", "x" );
            s.setValue( "joypad_up", "dpup" );
            s.setValue( "joypad_down", "dpdown" );
            s.setValue( "joypad_left", "dpleft" );
            s.setValue( "joypad_right", "dpright" );
            //s.setValue("joypad_leftstick", "leftstick");
            //s.setValue("joypad_rightstick", "rightstick");
            s.setValue( "joypad_l", "leftshoulder" );
            s.setValue( "joypad_r", "rightshoulder" );
            s.setValue( "joypad_select", "back" );
            s.setValue( "joypad_start", "start" );

            sdl_mapping = mappingForPort( current_port );
        }

        devices.insert( current_port, InputDeviceFactory::createFromMapping( sdl_mapping ) );
        current_port++;


    }

    QString message;

    if( joysticks == 0 ) {
        message = "No controllers were found.";
    } else {
        message = joysticks + "Controllers Found.";
    }

    qCDebug( phxInput ) << message;
    emit label( message );
}

void InputManager::scanDevices() {

    scanKeyboard();
    scanJoysticks();

    for( int i = 0; i < devices.length(); ++i ) {
        devices.at( i )->moveToThread( this->thread() );
    }

    // NOTES: some of the buttons in joystick.cpp line 216 aren't having proper values.
}

// load Mapping for a given port from the settings
// TODO: move some of this logic to the InputDeviceMapping class
InputDeviceMapping *InputManager::mappingForPort( unsigned port ) {
    QSettings s;
    s.beginGroup( "input" );
    s.beginGroup( QString( "port%1" ).arg( port ) );

    // create InputDeviceMapping subclass for the matching driver
    QString input_driver = s.value( "input_driver" ).toString();
    auto *mapping = InputDeviceMappingFactory::createMapping( input_driver );

    if( mapping == nullptr ) {
        return nullptr;
    }

    if( !mapping->populateFromSettings( s ) ) {
        delete mapping;
        return nullptr;
    }

    return mapping;
}

void InputManager::attachDevices() {
    for( int i = 0; i < devices.length(); ++i ) {
        InputDevice *device = devices.at( i );
        \

        if( device == nullptr ) {
            break;
        }

        top_window = QGuiApplication::topLevelWindows()[0];
        top_window->installEventFilter( device );
        settings_window = QGuiApplication::allWindows()[0];
        settings_window->installEventFilter( device );
    }

}

void InputManager::removeDevices() {
    for( int i = 0; i < devices.length(); ++i ) {
        InputDevice *device = devices.at( i );

        if( device == nullptr ) {
            break;
        }

        top_window->removeEventFilter( device );
        settings_window->removeEventFilter( device );

    }
}
