
#ifndef INPUTDEVICEMAPPING_H
#define INPUTDEVICEMAPPING_H

#include <unordered_map>

#include <QSettings>
#include <QJSValue>
#include <QMap>
#include <QVariantMap>
#include <memory>
#include <QList>

#include "libretro_types.h"
#include "inputdevice.h"
#include "inputdeviceevent.h"


// Class to map input from some device (eg keyboard, mouse, joystick)
// to retro_device_id events for the given retro_device_type.

class MappingModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString deviceEvent READ deviceEvent WRITE setDeviceEvent NOTIFY deviceEventChanged)
    Q_PROPERTY(QString retroID READ retroID WRITE setRetroID NOTIFY retroIDChanged)
    Q_PROPERTY(bool updating READ updating WRITE setUpdating NOTIFY updatingChanged)

public:


    MappingModel(QString devEvent, QString id)
    {
        setDeviceEvent(devEvent);
        setRetroID(id);
    }

    void setDeviceEvent(QString deviceEvent)
    {
        m_device_event = deviceEvent;
        emit deviceEventChanged();
    }

   void setRetroID(QString id)
   {
       m_retro_id = id;
       emit retroIDChanged();
   }

   void setUpdating(bool update)
   {
       m_updating = update;
       emit updatingChanged();
   }

   QString deviceEvent()
   {
       return m_device_event;
   }

   QString retroID()
   {
       return m_retro_id;
   }

   bool updating()
   {
       return m_updating;
   }

signals:
    void deviceEventChanged();
    void retroIDChanged();
    void updatingChanged();

private:
    QString m_retro_id;
    QString m_device_event;
    bool m_updating = false;

};


class InputDeviceMapping : public QObject {
        Q_OBJECT

    public:
        InputDeviceMapping();
        virtual ~InputDeviceMapping() { }

        virtual bool isValid();

        // takes a QSetting whose current group has been pre-set
        // by the caller to input/portX
        virtual bool populateFromSettings( QSettings &settings );

        // populate a Mapping from a QVariantMap as returned
        // by InputDevice::enumerateDevices
        virtual bool populateFromDict( QVariantMap deviceinfo );

        virtual InputDeviceEvent *eventFromString( QString ) = 0;

        retro_device_type deviceType() const {
            return device_type;
        }
        void setDeviceType( retro_device_type type ) {
            device_type = type;
        }


        retro_device_id getMapping( InputDeviceEvent *ev, retro_device_id defaultV = ~0 ) const {
            auto res = mapping.find( ev );

            if( res != mapping.end() ) {
                return res->second;
            }

            return defaultV;
        }

        void setMapping( InputDeviceEvent *ev, retro_device_id id, unsigned port );

        void setMapping( const InputDeviceEvent *ev, retro_device_id id ) {
            mapping[ev->clone()] = id;
        }

    public slots:
        // wait for any input on the underlying device and maps
        // the input id to the given retro_device_id
        virtual QVariant setMappingOnInput( retro_device_id id, QJSValue cb ) = 0;
        virtual void cancelMappingOnInput( QVariant cancelInfo ) = 0;
        virtual QString getMappingByRetroId( QString retroId );
        virtual void remapMapping( QString previousEvent, QVariant event, QString retroId, unsigned port );

        QString getGamepadName(QString retroID);
        QString variantToString(QVariant event)
        {
            return QString(*(event.value<InputDeviceEvent *>()));

        }

        bool collisionDetected(QString event, unsigned port);

        void saveModel(unsigned port);


        QList<QObject *> model();
        void updateModel();


    protected:
        // Type of retro device mapped:
        // JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
        // see libretro.h
        retro_device_type device_type;

        std::unordered_map<InputDeviceEvent *, retro_device_id> mapping;

private:

        QList<QObject *> m_model;

        void deleteModel();

signals:
        void modelChanged();

};

// to be able to use Connection objects in QVariants
Q_DECLARE_METATYPE( QMetaObject::Connection )


#endif
