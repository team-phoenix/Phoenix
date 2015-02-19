#ifndef MAPPINGMODEL_H
#define MAPPINGMODEL_H

#include <QList>
#include <QString>
#include <QSettings>
#include <QQmlContext>

#include <unordered_map>
#include "inputdevicemapping.h"

class ExposedMapping : public QObject  {
    Q_OBJECT
    Q_PROPERTY(QString deviceEvent READ deviceEvent WRITE setDeviceEvent NOTIFY deviceEventChanged)
    Q_PROPERTY(QString retroID READ retroID WRITE setRetroID NOTIFY retroIDChanged)
    Q_PROPERTY(bool updating READ updating WRITE setUpdating NOTIFY updatingChanged)

public:

    ExposedMapping(QString devEvent, QString id)
    {
        m_device_event = devEvent;
        m_retro_id = id;
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

   QString deviceEvent() const
   {
       return m_device_event;
   }

   QString retroID() const
   {
       return m_retro_id;
   }

   bool updating() const
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


class MappingModel : public QObject {
    Q_OBJECT
public:

    MappingModel(QObject *parent=0);
    ~MappingModel();

    void setContext(QQmlContext *context);
    void addMapping(QObject *mapping);




public slots:
    QList<QObject *> model()
    {
        return m_list;
    }
    void removeMapping(int index);

    void setDeviceMap(InputDeviceMapping *device_map);
    bool collisionDetected(QString event, unsigned port);

    void update();
    void saveModel(unsigned port);


signals:
    void itemAdded();
    void itemDeleted();
    void beforeItemAdded();
    void beforeItemDeleted();


protected:
    QHash<int, QByteArray> roleNames() const;

private:
    std::unordered_map<InputDeviceEvent *, retro_device_id> *m_map_data;
    QList<QObject *> m_list;
    QQmlContext *m_qml_context;

    void populateModel();
    void deleteAll();


};

#endif // MAPPINGMODEL_H
