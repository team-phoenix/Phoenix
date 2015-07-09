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

MetaDataDatabase::MetaDataDatabase( QObject *parent )
    : QObject( parent ),
      mProgress( 0.0 ),
      mCancel( false ) {

    open();

}

MetaDataDatabase::~MetaDataDatabase() {
    db.close();
}

void MetaDataDatabase::open() {
    auto currentDir = QDir::current().path();

    db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "METADATA" ) );

    db.setDatabaseName( currentDir + QDir::separator() + QStringLiteral( "openvgdb.sqlite" ) );

    if( !db.open() ) {
        qFatal( "Could not open database METADATA %s",
                qPrintable( db.lastError().driverText() ) );
    }

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );


}

QSqlDatabase &MetaDataDatabase::database() {
    return db;
}

QString MetaDataDatabase::getCheckSum( const QString filePath ) {
    QString hash;
    QFile file( filePath );

    if( file.open( QIODevice::ReadOnly ) ) {

        QCryptographicHash checkSum( QCryptographicHash::Sha1 );
        checkSum.addData( &file );
        hash = checkSum.result().toHex().toUpper();

        file.close();
    }

    return std::move( hash );
}

void MetaDataDatabase::getMetadata( GameMetaData metaData ) {

    mMutex.lock();
    bool cancelled = mCancel;
    mMutex.unlock();

    if( cancelled ) {
        QThread::currentThread()->exit();
        return;
    }

    auto checkSum = getCheckSum( metaData.filePath );

    if( checkSum.isEmpty() ) {
        return;
    }

    static const QString romIDFetchStatement = QStringLiteral( "SELECT romID FROM " )
                                             + tableRoms
                                             + QStringLiteral( " WHERE romHashSHA1 = ?" );

    sqlMutex.lock();
    QSqlQuery query( database() );
    sqlMutex.unlock();

    query.prepare( romIDFetchStatement );
    query.addBindValue( checkSum );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Metadata select error: "
                                << query.lastError().text();
        return;
    }

    // Get all of the rowID's to lookup in the RELEASES table.
    auto romID = -1;

    if( query.first() ) {
        romID = query.value( 0 ).toInt();
    }

    metaData.updated = ( romID != -1 );

    if( metaData.updated ) {

        query.clear();

        static const QString artworkFetchStatement = QStringLiteral(
                    "SELECT releaseCoverFront, releaseDescription, releaseDeveloper, releaseGenre ")
                    + QStringLiteral( " FROM " ) + MetaDataDatabase::tableReleases
                    + QStringLiteral(" WHERE romID = ?" );

        query.prepare( artworkFetchStatement );
        query.addBindValue( romID );

        if( !query.exec() ) {
            qCWarning( phxLibrary ) << "Metadata select error: "
                                    << query.lastError().text();
            return;
        }

        if( query.first() ) {

            metaData.artworkUrl = query.value( 0 ).toString();
            metaData.description = query.value( 1 ).toString();
            metaData.developer = query.value( 2 ).toString();
            metaData.genre = query.value( 3 ).toString();
            metaData.sha1 = checkSum;

        }
    }

    emit updateMetadata( std::move( metaData ) );

}

void MetaDataDatabase::setCancel( const bool cancel ) {
    mMutex.lock();
    mCancel = cancel;
    mMutex.unlock();
}
