
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>

#include "librarydbmanager.h"
#include "logging.h"


QSqlDatabase &LibraryDbManager::handle()
{
    if (!db.isValid())
        open();
    return db;
}

void LibraryDbManager::open()
{
    db = QSqlDatabase::addDatabase("QSQLITE");

    QString dataPathStr = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    Q_ASSERT(!dataPathStr.isEmpty());

    QDir dataPath(dataPathStr);
    auto appname = QCoreApplication::applicationName();
    if (!dataPath.exists(appname)) {
        dataPath.mkdir(appname); // race...
    }
    Q_ASSERT(dataPath.cd(appname));

    auto dbPath = dataPath.filePath(database_name);

    db.setDatabaseName(dbPath);
    qCDebug(phxLibrary, "Opening library database %s", qPrintable(db.databaseName()));
    if (!db.open()) {
        qFatal("Could not open database %s: %s", qPrintable(dbPath),
               qPrintable(db.lastError().driverText()));
    }

    if (!db.tables().contains(table_version)) {
        // must initialize db
        createSchema();
        //loadFixtures();
    }
}

bool LibraryDbManager::createSchema()
{
    qCDebug(phxLibrary, "Initializing database schema");
    db.transaction();
    QSqlQuery q(db);
    q.exec("CREATE TABLE " table_version " (version INTEGER NOT NULL)");
    q.exec("INSERT INTO " table_version " (version) VALUES (0)");
    q.exec("CREATE TABLE " table_games " (\n"
    "   id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
    "   /* game info */"
    "   title TEXT NOT NULL,\n"
    "   is_favorite BOOLEAN,\n"
    "   system TEXT NOT NULL,\n"
    "   region TEXT,\n"
    "   goodtools_code TEXT,\n"
    "   time_played DATETIME,\n"
    "   artwork TEXT,\n"
    "   /* file info */"
    "   directory TEXT,\n"
    "   filename TEXT UNIQUE,\n"
    "   system_path TEXT\n"
    ")");
    q.exec("CREATE INDEX title_index ON " table_games " (title)");
    q.exec("CREATE INDEX favorite_index ON " table_games " (is_favorite)");
    db.commit();
    return true;
}

bool LibraryDbManager::loadFixtures()
{
    qCDebug(phxLibrary, "Loading fixtures");
    db.transaction();
    QSqlQuery q(db);
    for (int i = 0; i < 10000; i++) {
        q.exec(QString("INSERT INTO " table_games " (title, system, time_played, artwork)"
                       " VALUES (\"somegame %1\", \"test\", \"0h 0m 0s\", \"qrc:/assets/missing_artwork.png\")").arg(i));
    }
    db.commit();
    return true;
}

int LibraryDbManager::version() const
{
    QSqlQuery q("SELECT version FROM " table_version " LIMIT 0,1", db);
    if(!q.exec()) {
        qCDebug(phxLibrary, "Error while retreiving db version: %s",
                            qPrintable(q.lastError().text()));
        q.lastError();
    }
    return q.value(0).toInt();
}

bool LibraryDbManager::isInitialized() const
{
    return db.isValid();
}
