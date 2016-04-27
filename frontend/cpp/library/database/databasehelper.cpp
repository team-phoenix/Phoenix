#include "databasehelper.h"

#include "logging.h"
#include "phxpaths.h"

#include <QtCore>
#include <QtSql>

using namespace Library;

QSqlDatabase Library::getLibretroDatabase() {
    QSqlDatabase libretroDatabase;

    // Create connection if connection not already created
    {
        QString connectionName = QThread::currentThread()->objectName() % QStringLiteral( "libretro" );

        if( !QSqlDatabase::contains( connectionName ) ) {
            libretroDatabase = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), connectionName );
        } else {
            libretroDatabase = QSqlDatabase::database( connectionName );
        }
    }

    if( !libretroDatabase.isOpen() ) {
        libretroDatabase.setDatabaseName( PhxPaths::metadataLocation() % QStringLiteral( "/libretro.sqlite" ) );
        Q_ASSERT( libretroDatabase.open() );
    }

    return libretroDatabase;
}

QSqlDatabase Library::getUserDatabase() {
    QSqlDatabase userDatabase;

    // Create connection if connection not already created
    {
        QString connectionName = QThread::currentThread()->objectName() % QStringLiteral( "userdata" );

        if( !QSqlDatabase::contains( connectionName ) ) {
            userDatabase = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), connectionName );
        } else {
            userDatabase = QSqlDatabase::database( connectionName );
        }
    }

    // Open if not already opened
    if( !userDatabase.isOpen() ) {
        userDatabase.setDatabaseName( PhxPaths::userDataLocation() % QStringLiteral( "/userdata.sqlite" ) );
        Q_ASSERT( userDatabase.open() );
    }

    // Check if schema is set up, initialize it if it's not
    {
        if( !userDatabase.tables().contains( "schema_version" ) ) {
            createUserDatabaseSchema( userDatabase );
        }
    }
    return userDatabase;
}

bool Library::createUserDatabaseSchema( QSqlDatabase db ) {
    qCDebug( phxLibrary, "Initializing database schema" );
    db.transaction();

    qCDebug( phxLibrary ) << db;

    QSqlQuery q( db );
    q.exec( "CREATE TABLE schema_version (version INTEGER NOT NULL)" );
    q.exec( QStringLiteral( "INSERT INTO schema_version (version) VALUES (0)" ) );
    q.exec( QStringLiteral( "CREATE TABLE games (\n" )
            % QStringLiteral( "   rowIndex INTEGER PRIMARY KEY AUTOINCREMENT,\n" )

            % QStringLiteral( "   \n/* game info */\n" )
            % QStringLiteral( "   title TEXT NOT NULL,\n" )
            % QStringLiteral( "   system TEXT,\n" )
            % QStringLiteral( "   region TEXT,\n" )
            % QStringLiteral( "   goodtoolsCode TEXT,\n" )
            % QStringLiteral( "   timePlayed DATETIME,\n" )
            % QStringLiteral( "   artworkUrl TEXT,\n" )
            % QStringLiteral( "   coreFilePath TEXT,\n" )

            % QStringLiteral( "   /* file info */" )
            % QStringLiteral( "   absolutePath TEXT,\n" )
            % QStringLiteral( "   absoluteFilePath TEXT UNIQUE NOT NULL,\n" )
            % QStringLiteral( "   crc32Checksum TEXT\n" )
            % QStringLiteral( ")" ) );

    q.exec( QStringLiteral( "CREATE INDEX title_index ON games (title)" ) );
    q.exec( QStringLiteral( "CREATE INDEX favorite_index ON games (is_favorite)" ) );


    // Create Collections Mapping Table
    q.exec( QStringLiteral( "CREATE TABLE collections (\n" )
            % QStringLiteral( " collectionID INTEGER PRIMARY KEY AUTOINCREMENT,\n" )
            % QStringLiteral( " collectionName TEXT UNIQUE NOT NULL\n" )
            % QStringLiteral( ")" ) );

    // Create Collections Table
    q.exec( QStringLiteral( "CREATE TABLE collectionMappings (\n" )
            % QStringLiteral( " collectionID INTEGER,\n" )
            % QStringLiteral( " rowIndex INTEGER,\n" )
            % QStringLiteral( " FOREIGN KEY (collectionID) REFERENCES collections " )
            % QStringLiteral( "(collectionID) ON DELETE CASCADE ON UPDATE CASCADE\n" )
            % QStringLiteral( " FOREIGN KEY (rowIndex) REFERENCES games " )
            % QStringLiteral( "(rowIndex) ON DELETE CASCADE ON UPDATE CASCADE\n" )
            % QStringLiteral( ")" ) );

    q.exec( QStringLiteral( "INSERT INTO collections " )
            % QStringLiteral( "(collectionID, collectionName) VALUES (0, 'All')" ) );

    // Create default core table
    q.exec( QStringLiteral( "CREATE TABLE defaultCores (\n" )
            % QStringLiteral( " system TEXT UNIQUE NOT NULL," )
            % QStringLiteral( " defaultCore TEXT" )
            % QStringLiteral( ")" ) );

    db.commit();

    return true;
}
