#include "metadatadatabase.h"
#include "logging.h"

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

    auto currentDir = QDir::current().path();

    auto db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "METADATA" ) );

    QString dataPathStr = QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation );
    Q_ASSERT( !dataPathStr.isEmpty() );

    QDir dataPath( dataPathStr );
    auto appname = QApplication::applicationName();

    if( !dataPath.exists( appname ) ) {
        dataPath.mkdir( appname ); // race...
    }

    Q_ASSERT( dataPath.cd( appname ) );

    db.setDatabaseName( currentDir + QDir::separator() + QStringLiteral( "openvgdb.sqlite" ) );

    if( !db.open() ) {
        qFatal( "Could not open database METADATA %s",
                qPrintable( db.lastError().driverText() ) );
    }

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );

}

QSqlDatabase MetaDataDatabase::database() {
    return QSqlDatabase::database( QStringLiteral( "METADATA" ) );
}
