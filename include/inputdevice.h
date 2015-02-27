#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include "libretro.h"

#include "libretro_types.h"
#include "inputdeviceevent.h"
#include "logging.h"


class InputDeviceMapping;


class InputDevice : public QObject {
        Q_OBJECT

    public:
        InputDevice( InputDeviceMapping *mapping );

        virtual ~InputDevice();

        int16_t state( retro_device_id id ) const
        {
            QMutexLocker lock( &ids_state_mutex );
            return ids_state.value( id, 0 );
        }

    signals:
        void inputEventReceived(InputDeviceEvent *ev, int16_t value);
        void deviceAdded(InputDevice *device);

    public slots:
        QString deviceName() const
        {
            return device_name;
        }
        InputDeviceMapping *mapping()
        {
            return m_mapping;
        }

        void deleteEventPtr(InputDeviceEvent *ev);

    protected:
        void setDeviceName( const char *new_name );
        void setState( retro_device_id id, int16_t state )
        {
            QMutexLocker lock( &ids_state_mutex );
            ids_state[id] = state;
        }

        // mapping
        InputDeviceMapping *m_mapping;

        // maps ids to state
        // ids can refer to a button, an axis, etc...
        // depending to the retro_device_type
        QMap<retro_device_id, int16_t> ids_state;
        mutable QMutex ids_state_mutex;

    private:
        // input device name, e.g "Xbox 360 Controller"
        QString device_name;

        retro_device_type m_type;
};

#endif // INPUTDEVICE_H
