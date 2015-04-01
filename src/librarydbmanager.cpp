
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>

#include "librarydbmanager.h"
#include "logging.h"
#include "systemdatabase.h"


static const QString table_version = QStringLiteral( "schema_version" );
static const QString database_name = QStringLiteral( "gamelibrary.sqlite" );

const QString LibraryDbManager::table_games = QStringLiteral( "games" );

QSqlDatabase &LibraryDbManager::handle() {
    if( !db.isValid() ) {
        open();
    }

    return db;
}

void LibraryDbManager::open() {
    db = QSqlDatabase::addDatabase( "QSQLITE", "first" );

    QString dataPathStr = QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation );
    Q_ASSERT( !dataPathStr.isEmpty() );

    QDir dataPath( dataPathStr );
    auto appname = QCoreApplication::applicationName();

    if( !dataPath.exists( appname ) ) {
        dataPath.mkdir( appname ); // race...
    }

    Q_ASSERT( dataPath.cd( appname ) );

    auto dbPath = dataPath.filePath( database_name );

    db.setDatabaseName( dbPath );
    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );

    if( !db.open() ) {
        qFatal( "Could not open database %s: %s", qPrintable( dbPath ),
                qPrintable( db.lastError().driverText() ) );
    }

    if( !db.tables().contains( table_version ) ) {
        // must initialize db
        createSchema();
        //loadFixtures();
    }
}

bool LibraryDbManager::createSchema() {
    qCDebug( phxLibrary, "Initializing database schema" );
    db.transaction();
    QSqlQuery q( db );
    q.exec( QStringLiteral( "CREATE TABLE " ) % table_version % QStringLiteral( " (version INTEGER NOT NULL)" ) );
    q.exec( QStringLiteral( "INSERT INTO " ) % table_version % QStringLiteral( " (version) VALUES (0)" ) );
    q.exec( QStringLiteral( "CREATE TABLE " ) % table_games % QStringLiteral( " (\n" ) %
            QStringLiteral( "   id INTEGER PRIMARY KEY AUTOINCREMENT,\n" ) %

            QStringLiteral( "   /* game info */" ) %
            QStringLiteral( "   title TEXT NOT NULL,\n" ) %
            QStringLiteral( "   is_favorite BOOLEAN,\n" ) %
            QStringLiteral( "   system TEXT NOT NULL,\n" ) %
            QStringLiteral( "   region TEXT,\n" ) %
            QStringLiteral( "   goodtools_code TEXT,\n" ) %
            QStringLiteral( "   time_played DATETIME,\n" ) %
            QStringLiteral( "   artwork TEXT,\n" ) %

            QStringLiteral( "   /* file info */" ) %
            QStringLiteral( "   directory TEXT,\n" ) %
            QStringLiteral( "   filename TEXT UNIQUE,\n" ) %
            QStringLiteral( "   sha1 BLOB,\n" ) %
            QStringLiteral( "   crc32 BLOB,\n" ) %
            QStringLiteral( "   rom_count BOOLEAN\n" ) %
            QStringLiteral( ")" ) );

    q.exec( QStringLiteral( "CREATE INDEX title_index ON " ) % table_games % QStringLiteral( " (title)" ) );
    q.exec( QStringLiteral( "CREATE INDEX favorite_index ON " ) % table_games % QStringLiteral( " (is_favorite)" ) );
    db.commit();
    return true;
}

bool LibraryDbManager::loadFixtures() {
    qCDebug( phxLibrary, "Loading fixtures" );
    db.transaction();
    QSqlQuery q( db );
    const QString command = QStringLiteral( "INSERT INTO " ) % table_games % QStringLiteral( " (title, system, time_played, artwork)" ) %
                            QStringLiteral( " VALUES (\"somegame %2\", \"test\", \"0h 0m 0s\", \"qrc:/assets/No-Art.png\")" );

    for( int i = 0; i < 10000; i++ ) {
        q.exec( command.arg( i ) );
    }

    db.commit();
    return true;
}

int LibraryDbManager::version() const {
    QSqlQuery q( QStringLiteral( "SELECT version FROM " ) % table_version % QStringLiteral( " LIMIT 0,1" ), db );

    if( !q.exec() ) {
        qCDebug( phxLibrary, "Error while retreiving db version: %s",
                 qPrintable( q.lastError().text() ) );
        q.lastError();
    }

    return q.value( 0 ).toInt();
}

bool LibraryDbManager::isInitialized() const {
    return db.isValid();
}
