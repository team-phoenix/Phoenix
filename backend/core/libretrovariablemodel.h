#pragma once

#include "libretrovariable.h"

#include <QAbstractTableModel>
#include <QHash>
#include <QList>
#include <QMap>
#include <QObject>

class LibretroVariableForwarder;

class LibretroVariableModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        enum Role {
            Key = Qt::UserRole + 1,
            Choices,
            Description,
        };

        LibretroVariableModel( QObject *parent = nullptr );

        QHash<int, QByteArray> roleNames() const;
        int rowCount( const QModelIndex &parent ) const;
        int columnCount( const QModelIndex &parent ) const;
        QVariant data( const QModelIndex &index, int role ) const;

        void setForwarder( LibretroVariableForwarder *forwarder );

    signals:
        void setVariable( QString key, QString value );

    public slots:
        void clearVariables();
        void insertVariable( QString key, QStringList values, QString currentValue, QString description );
        void updateVariable( QString key, QString value );

    private:
        LibretroVariableForwarder *forwarder { nullptr };

        QHash<int, QByteArray> roles;

        QMap<QString, QStringList> values;
        QMap<QString, QString> currentValues;
        QMap<QString, QString> descriptions;
};
