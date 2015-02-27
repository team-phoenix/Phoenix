#ifndef MAPPINGMODEL_H
#define MAPPINGMODEL_H

#include <QList>
#include <QString>
#include <QSettings>
#include <QQmlContext>

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
    void insertMapping(unsigned insert, QObject *mapping);

public slots:
    QList<QObject *> model()
    {
        return m_list;
    }
    void removeMapping(int index);

signals:
    void itemAdded();
    void itemDeleted();
    void beforeItemAdded();
    void beforeItemDeleted();

private slots:
    void deleteAll();

private:
    QQmlContext *m_qml_context;
    QList<QObject *> m_list;

};

#endif // MAPPINGMODEL_H
