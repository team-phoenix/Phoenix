
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QList>
#include <QObject>
#include <QWindow>
#include <QQmlContext>

#include "inputdevice.h"
#include "inputdevicemapping.h"
#include "mappingmodel.h"


class InputManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool attachDevices READ attachDevices WRITE setAttachDevices NOTIFY attachDevicesChanged)
    Q_PROPERTY(bool findingDevices READ findingDevices WRITE setFindingDevices NOTIFY findingDevicesChanged)

    public:
        InputManager();
        virtual ~InputManager();

        void append( InputDevice *device );

        QList<InputDevice *> getDevices() const;

        bool attachDevices() const;
        bool findingDevices() const;
        void setFindingDevices(bool findDevices);
        void setAttachDevices(bool attachDevices);

    public slots:
        void scanDevicesAsync();
        void scanDevices();
        void scanJoysticks();
        void scanKeyboard();

        QVariantList enumerateDevices();
        // return empty mapping for device
        InputDeviceMapping *mappingForDevice(QVariantMap device);

        // load existing mapping for designated port from settings
        InputDeviceMapping *mappingForPort(unsigned port);

        InputDevice *getDevice(unsigned port) const;

        void handleAttachDevices();
        void removeDevices();
        QString variantToString(QVariant event);
        bool swap(int index, int index_2);
        void setContext(QQmlContext *context);
        void updateModel();
        int count();

    signals:
        void label( QString );
        void updateChanged();
        void countChanged();
        void attachDevicesChanged();
        void findingDevicesChanged();

    private:
        QList<InputDevice *> devices;
        QWindow *top_window;
        QWindow *settings_window;
        QQmlContext *m_context;

        bool m_attach_devices;
        bool m_finding_devices;

};

#endif
