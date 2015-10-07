#include "metadatadatabase.h"
#include "logging.h"
#include "phxpaths.h"

#include <QCryptographicHash>
#include <QFile>
#include <QThread>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

using namespace Library;

const QString MetaDataDatabase::tableRoms = QStringLiteral( "ROMs" );
const QString MetaDataDatabase::tableSystems = QStringLiteral( "SYSTEMS" );
const QString MetaDataDatabase::tableReleases = QStringLiteral( "RELEASES" );
const QString MetaDataDatabase::tableRegions = QStringLiteral( "REGIONS" );

void MetaDataDatabase::open() {

    auto db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "METADATA" ) );

    //#######################
    QString dataPathStr = PhxPaths::path( PhxPaths::DatabasesLocation );
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
