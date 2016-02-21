#include "libretrodatabase.h"

using namespace Library;

QMutex LibretroDatabase::mutex;

QSqlDatabase LibretroDatabase::database() {
    return QSqlDatabase::database( QStringLiteral( "SYSTEMS" ) );
}

void LibretroDatabase::close( const ThreadMode mode ) {
    if( QSqlDatabase::contains( QStringLiteral( "SYSTEMS" ) ) ) {
        qDebug( phxLibrary ) << "closing SYSTEM database";
        auto db = QSqlDatabase::database( QStringLiteral( "SYSTEMS" ) );
        if ( db.isOpen() ) {
            db.close();
        }
    }

    if ( mode == ThreadMode::NeedsMutex ) {
        mutex.unlock();
    }
}

void LibretroDatabase::addDatabase() {
    QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "SYSTEMS" ) );
}

void LibretroDatabase::removeDatabase() {
    QSqlDatabase::removeDatabase( QStringLiteral( "SYSTEMS" ) );
}

void LibretroDatabase::open( const ThreadMode mode ) {

    if ( mode == ThreadMode::NeedsMutex ) {
        mutex.lock();
    }

    {
        QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "SYSTEMS" ) );
        if ( db.isOpen() && !db.databaseName().isEmpty() ) {
            return;
        }
    }

    QSqlDatabase db = QSqlDatabase::database( QStringLiteral( "SYSTEMS" ) );

    QString dataPathStr = PhxPaths::metadataLocation();
    Q_ASSERT_X( !dataPathStr.isEmpty(), Q_FUNC_INFO, "dataPathStr.isEmpty()" );

    QDir dataPath( dataPathStr );

    auto databaseName = QStringLiteral( "libretro.sqlite" );
    auto filePath = dataPath.filePath( databaseName );

    db.setDatabaseName( filePath );

    auto open = db.open();
    Q_ASSERT_X( open, Q_FUNC_INFO, "db.open() == false" ) ;

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );
}
