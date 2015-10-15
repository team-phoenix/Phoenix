#include "libraryinternaldatabase.h"

using namespace Library;

const QString LibraryInternalDatabase::tableVersion = QStringLiteral( "schema_version" );
const QString LibraryInternalDatabase::databaseName = QStringLiteral( "gamelibrary.sqlite" );
const QString LibraryInternalDatabase::tableName = QStringLiteral( "games" );
const QString LibraryInternalDatabase::tableCollectionMappings = QStringLiteral( "collectionMappings" );
const QString LibraryInternalDatabase::tableCollections = QStringLiteral( "collections" );

LibraryInternalDatabase::LibraryInternalDatabase() {
    open();
}

LibraryInternalDatabase *LibraryInternalDatabase::instance() {
    static LibraryDatabasePointer instance( new LibraryInternalDatabase );
    return instance.get();
}

QSqlDatabase &LibraryInternalDatabase::database() {
    return db;
}
void LibraryInternalDatabase::close() {
    db.close();
}

void LibraryInternalDatabase::open() {
    if( !db.isValid() ) {
        db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "LIBRARY" ) );

        QString dataPathStr = PhxPaths::userDataLocation();
        Q_ASSERT( !dataPathStr.isEmpty() );

        QDir dataPath( dataPathStr );

        mFilePath = dataPath.filePath( databaseName );

        db.setDatabaseName( mFilePath );
        qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );
    }

    if( !db.open() ) {
        qFatal( "Could not open database %s: %s", qPrintable( mFilePath ),
                qPrintable( db.lastError().driverText() ) );
    }

    if( !db.tables().contains( tableVersion ) ) {
        // must initialize db
        createSchema();

        // Only uncomment this when you're testing.
        //loadFixtures();
    }
}

bool LibraryInternalDatabase::createSchema() {

    qCDebug( phxLibrary, "Initializing database schema" );
    db.transaction();

    QSqlQuery q( db );
    q.exec( "CREATE TABLE " + LibraryInternalDatabase::tableVersion + " (version INTEGER NOT NULL)" );
    q.exec( QStringLiteral( "INSERT INTO " ) + LibraryInternalDatabase::tableVersion + QStringLiteral( " (version) VALUES (0)" ) );
    q.exec( QStringLiteral( "CREATE TABLE " ) + LibraryInternalDatabase::tableName + QStringLiteral( " (\n" ) +
            QStringLiteral( "   rowIndex INTEGER PRIMARY KEY AUTOINCREMENT,\n" ) +

            QStringLiteral( "   \n/* game info */\n" ) +
            QStringLiteral( "   title TEXT NOT NULL,\n" ) +
            QStringLiteral( "   system TEXT,\n" ) +
            QStringLiteral( "   region TEXT,\n" ) +
            QStringLiteral( "   goodtoolsCode TEXT,\n" ) +
            QStringLiteral( "   timePlayed DATETIME,\n" ) +
            QStringLiteral( "   artworkUrl TEXT,\n" ) +
            QStringLiteral( "   coreFilePath TEXT,\n" ) +

            QStringLiteral( "   /* file info */" ) +
            QStringLiteral( "   absolutePath TEXT,\n" ) +
            QStringLiteral( "   absoluteFilePath TEXT NOT NULL,\n" ) +
            QStringLiteral( "   sha1 TEXT\n" ) +
            QStringLiteral( ")" ) );

    q.exec( QStringLiteral( "CREATE INDEX title_index ON " ) + LibraryInternalDatabase::tableName + QStringLiteral( " (title)" ) );
    q.exec( QStringLiteral( "CREATE INDEX favorite_index ON " ) + LibraryInternalDatabase::tableName + QStringLiteral( " (is_favorite)" ) );


    // Create Collections Mapping Table
    q.exec( QStringLiteral( "CREATE TABLE " ) + LibraryInternalDatabase::tableCollections + QStringLiteral( "(\n" ) +
            QStringLiteral( " collectionID INTEGER PRIMARY KEY AUTOINCREMENT,\n" ) +
            QStringLiteral( " collectionName TEXT UNIQUE NOT NULL\n" ) +
            QStringLiteral( ")" ) );




    // Create Collections Table
    q.exec( QStringLiteral( "CREATE TABLE " ) + LibraryInternalDatabase::tableCollectionMappings + QStringLiteral( "(\n" ) +
            QStringLiteral( " collectionID INTEGER,\n" ) +
            QStringLiteral( " rowIndex INTEGER,\n" )  +
            QStringLiteral( " FOREIGN KEY (collectionID) REFERENCES " ) + LibraryInternalDatabase::tableCollections +
            QStringLiteral( "(collectionID) ON DELETE CASCADE ON UPDATE CASCADE\n" ) +
            QStringLiteral( " FOREIGN KEY (rowIndex) REFERENCES " ) + LibraryInternalDatabase::tableName +
            QStringLiteral( "(rowIndex) ON DELETE CASCADE ON UPDATE CASCADE\n" ) +
            QStringLiteral( ")" ) );

    q.exec( QStringLiteral( "INSERT INTO " ) + LibraryInternalDatabase::tableCollections
            + QStringLiteral( " (collectionID, collectionName) VALUES (0, 'All')" ) );

    db.commit();

    return true;
}

bool LibraryInternalDatabase::loadFixtures() {
    qCDebug( phxLibrary, "Loading fixtures" );
    db.transaction();
    QSqlQuery q( db );
    const QString command = QStringLiteral( "INSERT INTO " )
                            + tableName
                            + QStringLiteral( " (title, system, time_played, artwork)" )
                            + QStringLiteral( " VALUES (\"somegame %2\", \"test\", \"0h 0m 0s\", \"qrc:/assets/No-Art.png\")" );

    for( int i = 0; i < 10000; i++ ) {
        q.exec( command.arg( i ) );
    }

    db.commit();
    return true;
}

int LibraryInternalDatabase::version() const {
    QSqlQuery q( QStringLiteral( "SELECT version FROM " )
                 + tableVersion
                 + QStringLiteral( " LIMIT 0,1" ), db );

    if( !q.exec() ) {
        qCDebug( phxLibrary, "Error while retreiving db version: %s",
                 qPrintable( q.lastError().text() ) );
        q.lastError();
    }

    return q.value( 0 ).toInt();
}

QString LibraryInternalDatabase::filePath() const {
    return mFilePath;
}
