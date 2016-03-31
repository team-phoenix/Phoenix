#pragma once

#include "frontendcommon.h"

// DatabaseSettings is a wrapper for setting the QSqlDatabase setters from QML.
// This is exposed in the SqlModel under the Q_PROPERTY called "databaseSettings" and is a read only object.
// The getters are use to fill in the QSqlDatabase information during "SqlModel::finishModelConstruction".
// All property names are taken almost exactly from QSqlDatabase.

class DatabaseSettings : public QObject {
        Q_OBJECT
        Q_PROPERTY( QString password READ password WRITE setPassword )
        Q_PROPERTY( QString userName READ userName WRITE setUserName )
        Q_PROPERTY( QString connectionOptions READ connectionOptions WRITE setConnectionOptions )
        Q_PROPERTY( QString databaseName READ databaseName WRITE setDatabaseName )
        Q_PROPERTY( QString driverType READ driverType WRITE setDriverType )
        Q_PROPERTY( QString hostName READ hostName WRITE setHostName )
        Q_PROPERTY( QString connectionName READ connectionName WRITE setConnectionName )

        // !![CAUTION]!! DatabaseSettings::port() is defaulted to INT_MIN, so you cannot use that as a port value.
        Q_PROPERTY( int port READ port WRITE setPort )
        // ~!![CAUTION]!!
        Q_PROPERTY( NumericalPrecisionPolicy numericalPrecisionPolicy READ numericalPrecisionPolicy WRITE setNumericalPrecisionPolicy )

    public:
        enum class NumericalPrecisionPolicy {
            Default = -1,
            LowPrecisionInt32 = QSql::LowPrecisionInt32,
            LowPrecisionInt64 = QSql::LowPrecisionInt64,
            LowPrecisionDouble = QSql::LowPrecisionDouble,
            HighPrecision = QSql::HighPrecision,
        };
        Q_ENUMS( NumericalPrecisionPolicy )

        DatabaseSettings( QObject *parent = nullptr );

        // [!!] QML Q_PROPERTY ( Getters )
        QString password() const;
        QString userName() const;
        QString connectionOptions() const;
        QString databaseName() const;
        QString driverType() const;
        QString hostName() const;
        QString connectionName() const;
        int port() const;
        NumericalPrecisionPolicy numericalPrecisionPolicy() const;
        // ~[!!]

        // [!!] QML Q_PROPERTY ( Setters )
        void setPassword( const QString password );
        void setUserName( const QString userName );
        void setConnectionOptions( const QString options );
        void setDatabaseName( const QString name );
        void setDriverType( const QString type );
        void setHostName( const QString name );
        void setConnectionName( const QString name );
        void setPort( const int port );
        void setNumericalPrecisionPolicy( const NumericalPrecisionPolicy policy );
        // ~[!!]

        Q_INVOKABLE static bool isDriverAvailable( const QString name );

    private:
        QString mPassword;
        QString mUserName;
        QString mConnectionOptions;
        QString mDatabaseName;
        QString mDriverType;
        QString mHostName;
        QString mConnectionName;

        int mPort;
        NumericalPrecisionPolicy mNumericalPrecisionPolicy;
};

QDebug operator <<( QDebug d, const DatabaseSettings &db );
