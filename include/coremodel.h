#ifndef COREMODEL_H
#define COREMODEL_H

#include <QString>
#include <QObject>

class CoreModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString corePath READ corePath WRITE setCorePath NOTIFY corePathChanged)
    Q_PROPERTY(QString coreName READ coreName WRITE setCoreName NOTIFY coreNameChanged)
public:
    CoreModel(QObject *parent=0, QString coreName="", QString corePath="");
    ~CoreModel();
    void setCorePath(QString corePath);
    void setCoreName(QString coreName);

    QString corePath()
    {
        return m_core_path;
    }

    QString coreName()
    {
        return m_core_name;
    }

signals:
    void corePathChanged();
    void coreNameChanged();
private:
    QString m_core_path;
    QString m_core_name;
};


#endif //COREMODEL_H
