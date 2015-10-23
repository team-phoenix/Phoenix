#include "userdatabase.h"

using namespace Library;

const QString UserDatabase::tableVersion = QStringLiteral( "schema_version" );
const QString UserDatabase::databaseName = QStringLiteral( "userdata.sqlite" );
const QString UserDatabase::tableName = QStringLiteral( "games" );
const QString UserDatabase::tableCollectionMappings = QStringLiteral( "collectionMappings" );
const QString UserDatabase::tableCollections = QStringLiteral( "collections" );
const QString UserDatabase::tableDefaultCores = QStringLiteral( "defaultCores" );

UserDatabase::UserDatabase() {
    open();
}

UserDatabase *UserDatabase::instance() {
    static UserDatabasePointer instance( new UserDatabase );
    return instance.get();
}

QSqlDatabase &UserDatabase::database() {
    return db;
}
void UserDatabase::close() {
    db.close();
}

void UserDatabase::open() {
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

bool UserDatabase::createSchema() {

    qCDebug( phxLibrary, "Initializing database schema" );
    db.transaction();

    qCDebug( phxLibrary ) << db;

    QSqlQuery q( db );
    q.exec( "CREATE TABLE " + UserDatabase::tableVersion + " (version INTEGER NOT NULL)" );
    q.exec( QStringLiteral( "INSERT INTO " ) + UserDatabase::tableVersion + QStringLiteral( " (version) VALUES (0)" ) );
    q.exec( QStringLiteral( "CREATE TABLE " ) + UserDatabase::tableName + QStringLiteral( " (\n" ) +
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
            QStringLiteral( "   absoluteFilePath TEXT UNIQUE NOT NULL,\n" ) +
            QStringLiteral( "   crc32Checksum TEXT\n" ) +
            QStringLiteral( ")" ) );

    q.exec( QStringLiteral( "CREATE INDEX title_index ON " ) + UserDatabase::tableName + QStringLiteral( " (title)" ) );
    q.exec( QStringLiteral( "CREATE INDEX favorite_index ON " ) + UserDatabase::tableName + QStringLiteral( " (is_favorite)" ) );


    // Create Collections Mapping Table
    q.exec( QStringLiteral( "CREATE TABLE " ) + UserDatabase::tableCollections + QStringLiteral( "(\n" ) +
            QStringLiteral( " collectionID INTEGER PRIMARY KEY AUTOINCREMENT,\n" ) +
            QStringLiteral( " collectionName TEXT UNIQUE NOT NULL\n" ) +
            QStringLiteral( ")" ) );

    // Create Collections Table
    q.exec( QStringLiteral( "CREATE TABLE " ) + UserDatabase::tableCollectionMappings + QStringLiteral( "(\n" ) +
            QStringLiteral( " collectionID INTEGER,\n" ) +
            QStringLiteral( " rowIndex INTEGER,\n" )  +
            QStringLiteral( " FOREIGN KEY (collectionID) REFERENCES " ) + UserDatabase::tableCollections +
            QStringLiteral( "(collectionID) ON DELETE CASCADE ON UPDATE CASCADE\n" ) +
            QStringLiteral( " FOREIGN KEY (rowIndex) REFERENCES " ) + UserDatabase::tableName +
            QStringLiteral( "(rowIndex) ON DELETE CASCADE ON UPDATE CASCADE\n" ) +
            QStringLiteral( ")" ) );

    q.exec( QStringLiteral( "INSERT INTO " ) + UserDatabase::tableCollections
            + QStringLiteral( " (collectionID, collectionName) VALUES (0, 'All')" ) );

    // Create default core table
    q.exec( QStringLiteral( "CREATE TABLE " ) + UserDatabase::tableDefaultCores + QStringLiteral( "(\n" ) +
            QStringLiteral( " system TEXT UNIQUE NOT NULL," ) +
            QStringLiteral( " defaultCore TEXT" ) +
            QStringLiteral( ")" ) );

    db.commit();

    return true;
}

bool UserDatabase::loadFixtures() {
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

int UserDatabase::version() const {
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

QString UserDatabase::filePath() const {
    return mFilePath;
}
