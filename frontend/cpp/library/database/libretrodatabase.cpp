#include "libretrodatabase.h"

using namespace Library;

QMutex LibretroDatabase::mutex;

LibretroDatabase::LibretroDatabase()
    : LibretroDatabase( NoMutex )
{
}

LibretroDatabase::LibretroDatabase(const LibretroDatabase::ThreadMode mode)
    : mMode( mode ) {
    open( mMode );
}

LibretroDatabase::~LibretroDatabase()
{
    close( mMode );
}

void LibretroDatabase::close( const ThreadMode mode ) {
    if( QSqlDatabase::contains( QStringLiteral( "SYSTEMS" ) ) ) {
        qDebug( phxLibrary ) << "closing SYSTEM database";
        QSqlDatabase db = database();
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

QSqlDatabase LibretroDatabase::database() const
{
    return QSqlDatabase::database( QStringLiteral( "SYSTEMS" ) );
}

void LibretroDatabase::open( const ThreadMode mode ) {

    if ( mode == ThreadMode::NeedsMutex ) {
        mutex.lock();
    }

    {
        QSqlDatabase db = database();
        if ( db.isOpen() && !db.databaseName().isEmpty() ) {
            return;
        }
    }

    QSqlDatabase db = database();

    QString dataPathStr = PhxPaths::metadataLocation();
    Q_ASSERT_X( !dataPathStr.isEmpty(), Q_FUNC_INFO, "dataPathStr.isEmpty()" );

    QDir dataPath( dataPathStr );

    QString databaseName = QStringLiteral( "libretro.sqlite" );
    QString filePath = dataPath.filePath( databaseName );

    db.setDatabaseName( filePath );

    bool open = db.open();
    Q_ASSERT_X( open, Q_FUNC_INFO, "db.open() == false" ) ;

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );
}
