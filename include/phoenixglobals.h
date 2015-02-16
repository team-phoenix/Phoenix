#ifndef PHOENIXGLOBALS_H
#define PHOENIXGLOBALS_H

#include <QObject>

class PhoenixGlobals : public QObject {
    Q_OBJECT
public:
    explicit PhoenixGlobals(QObject *parent=0) : QObject(parent)
    {

    }

    ~PhoenixGlobals()
    {

    }

    void setOfflineStoragePath(const QString &path)
    {
        m_offline_storage_path = path;
    }

public slots:
    QString offlineStoragePath()
    {
        return m_offline_storage_path;
    }

private:
    QString m_offline_storage_path;

};

#endif // PHOENIXGLOBALS_H
