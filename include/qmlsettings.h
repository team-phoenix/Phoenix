
#ifndef QMLSETTINGS_H
#define QMLSETTINGS_H

#include <QSettings>
#include <QDebug>


class QMLSettings : public QObject {
        Q_OBJECT

    public:
        QMLSettings() {};
        virtual ~QMLSettings() {};

    public slots:
        void setValue( const QString &key, const QVariant &value ) {
            QSettings s;
            s.setValue( key, value );
        }

        QVariant getValue( const QString &key, const QVariant &defaultValue = QVariant() ) {
            QSettings s;
            return s.value( key, defaultValue );
        }
};

#endif
