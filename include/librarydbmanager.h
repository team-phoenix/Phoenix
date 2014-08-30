
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>

#define table_version "schema_version"
#define table_games "games"

class LibraryDbManager
{
public:
    LibraryDbManager() {};
    ~LibraryDbManager() {}

    QSqlDatabase &handle();

    void open();

    bool isInitialized() const;

    int version() const;

private:
    LibraryDbManager(const LibraryDbManager &);
    LibraryDbManager &operator=(const LibraryDbManager &);
    QSqlDatabase db;

    bool createSchema();
    bool loadFixtures();

    constexpr static const char *database_name = "gamelibrary.sqlite";
};


#endif
