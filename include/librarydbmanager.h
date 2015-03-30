
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>

/* The LibraryDbManager is in charge of opening the sql database
 * for the GameLibraryModel.
 *
 * This LibraryDbManager class also will generate the sql schema.
 */

class LibraryDbManager {
    public:
        LibraryDbManager() {};
        ~LibraryDbManager() {}

        QSqlDatabase &handle();

        void open();

        bool isInitialized() const;

        int version() const;

        static const QString table_games;

    private:
        LibraryDbManager( const LibraryDbManager & );
        //LibraryDbManager &operator=(const LibraryDbManager &);
        QSqlDatabase db;

        bool createSchema();
        bool loadFixtures();

};


#endif
