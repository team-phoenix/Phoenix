#include "metadatadatabase.h"

using namespace Library;

const QString MetaDataDatabase::tableRoms = QStringLiteral( OPENVGDBTABLEROMS );
const QString MetaDataDatabase::tableSystems = QStringLiteral( OPENVGDBTABLESYSTEMS );
const QString MetaDataDatabase::tableReleases = QStringLiteral( OPENVGDBTABLERELEASES );
const QString MetaDataDatabase::tableRegions = QStringLiteral( OPENVGDBTABLEREGIONS );

void MetaDataDatabase::open() {

    auto db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "METADATA" ) );

    //#######################
    QString dataPathStr = PhxPaths::metadataLocation();
    Q_ASSERT( !dataPathStr.isEmpty() );

    QDir dataPath( dataPathStr );

    auto databaseName = QStringLiteral( "openvgdb.sqlite" );
    auto filePath = dataPath.filePath( databaseName );

    db.setDatabaseName( filePath );
    //#######################

    if( !db.open() ) {
        qFatal( "Could not open database METADATA %s",
                qPrintable( db.lastError().driverText() ) );
    }

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );

}

QSqlDatabase MetaDataDatabase::database() {
    return QSqlDatabase::database( QStringLiteral( "METADATA" ) );
}
