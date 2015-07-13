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

MetaDataDatabase::MetaDataDatabase() {

}

MetaDataDatabase::~MetaDataDatabase() {
}

void MetaDataDatabase::close() {
    db.close();
}


void MetaDataDatabase::open() {
    auto currentDir = QDir::current().path();

    if ( !db.isValid() )  {
        db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "METADATA" ) );

        db.setDatabaseName( currentDir + QDir::separator() + QStringLiteral( "openvgdb.sqlite" ) );
    }

    if( !db.open() ) {
        qFatal( "Could not open database METADATA %s",
                qPrintable( db.lastError().driverText() ) );
    }

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );


}

QSqlDatabase &MetaDataDatabase::database() {
    return db;
}
