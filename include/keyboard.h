#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <memory>

#include <QKeyEvent>

#include "inputdevice.h"
#include "inputdevicemapping.h"

/* The Keyboard class is an InputDevice that represents one keyboard.
 *
 * This class could exist inside of the InputManager's InputDevice list as long as any
 * keyboards are connected to the computer.
 */

class Keyboard : public InputDevice {
    public:
        Keyboard( InputDeviceMapping *mapping );
        virtual ~Keyboard();

        static QVariantList enumerateDevices();

        virtual bool eventFilter( QObject *obj, QEvent *event ) override;

        class Mapping : public InputDeviceMapping {
            public:
                Mapping() {}

                virtual InputDeviceEvent *eventFromString( QString ) override;

            public slots:
                virtual QVariant setMappingOnInput( retro_device_id id, QJSValue cb ) override;
                virtual void cancelMappingOnInput( QVariant cancelInfo ) override;

            private:
                // only used by setMappingOnInput helper function
                std::unique_ptr<Keyboard> keyboard;
        };

    private:
        // process QKeyEvent sent from some widget/window
        // as a button press in this virtual Input Device
        void processKeyEvent( QKeyEvent *event );
};


#endif // KEYBOARD_H
