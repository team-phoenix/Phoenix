#include "metadatadatabase.h"

using namespace Library;

const QString MetaDataDatabase::tableRoms = QStringLiteral( "ROMs" );
const QString MetaDataDatabase::tableSystems = QStringLiteral( "SYSTEMS" );
const QString MetaDataDatabase::tableReleases = QStringLiteral( "RELEASES" );
const QString MetaDataDatabase::tableRegions = QStringLiteral( "REGIONS" );

QMutex MetaDataDatabase::mutex;

void MetaDataDatabase::open( const ThreadMode mode ) {

    if( mode == ThreadMode::NeedsMutex ) {
        mutex.lock();
    }

    {
        QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );

        if( db.isOpen() && !db.databaseName().isEmpty() ) {
            return;
        }
    }

    QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );


    //#######################
    QString dataPathStr = PhxPaths::metadataLocation();
    Q_ASSERT( !dataPathStr.isEmpty() );

    QDir dataPath( dataPathStr );

    QString databaseName = QStringLiteral( "openvgdb.sqlite" );
    QString filePath = dataPath.filePath( databaseName );

    db.setDatabaseName( filePath );
    //#######################

    if( !db.open() ) {
        qFatal( "Could not open database METADATA %s",
                qPrintable( db.lastError().driverText() ) );
    }

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );

}

void MetaDataDatabase::close( const ThreadMode mode ) {

    if( QSqlDatabase::contains( "METADATA" ) ) {

        // Must be scoped. QSql is real picky about this.
        QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );

        if( db.isOpen() ) {
            db.close();
        }
    }

    if( mode == ThreadMode::NeedsMutex ) {
        mutex.unlock();
    }
}

void MetaDataDatabase::addDatabase() {
    QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "METADATA" ) );
}

void MetaDataDatabase::removeDatabase() {
    QSqlDatabase::removeDatabase( QStringLiteral( "METADATA" ) );
}

void MetaDataDatabase::addConnection( const QString &name ) {
    if( !QSqlDatabase::contains( name ) ) {
        qCDebug( phxLibrary ) << "Opening a new metadata DB SQL connection:" << name;

        QSqlDatabase db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), name );

        //#######################
        QString dataPathStr = PhxPaths::metadataLocation();
        Q_ASSERT( !dataPathStr.isEmpty() );

        QDir dataPath( dataPathStr );

        QString databaseName = QStringLiteral( "openvgdb.sqlite" );
        QString filePath = dataPath.filePath( databaseName );

        db.setDatabaseName( filePath );
    }
}

QSqlDatabase MetaDataDatabase::database() {
    QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );

    if( !db.isOpen() || !db.isValid() ) {
        MetaDataDatabase::open();
        db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );
    }

    return db;
}
