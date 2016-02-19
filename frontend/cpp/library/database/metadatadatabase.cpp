#include "metadatadatabase.h"

using namespace Library;

const QString MetaDataDatabase::tableRoms = QStringLiteral( OPENVGDBTABLEROMS );
const QString MetaDataDatabase::tableSystems = QStringLiteral( OPENVGDBTABLESYSTEMS );
const QString MetaDataDatabase::tableReleases = QStringLiteral( OPENVGDBTABLERELEASES );
const QString MetaDataDatabase::tableRegions = QStringLiteral( OPENVGDBTABLEREGIONS );

void MetaDataDatabase::open() {
    {
        QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );
        if ( db.isOpen() ) {
            return;
        }
    }

    QSqlDatabase db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "METADATA" ) );

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

void MetaDataDatabase::close() {
    if( QSqlDatabase::contains( "METADATA" ) ) {

        // Must be scoped. QSql is real picky about this.
        {
            QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );
            qDebug() << "contains db : " << db.isOpen() << ", " << db.isValid();

            db.close();
        }

        //QSqlDatabase::removeDatabase( "METADATA" );

    }
}

QSqlDatabase MetaDataDatabase::database() {
    QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );
    if ( !db.isOpen() || !db.isValid() ) {
        MetaDataDatabase::open();
        db = QSqlDatabase::database( QStringLiteral( "METADATA" ) );
    }
    return db;
}
