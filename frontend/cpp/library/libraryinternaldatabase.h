#ifndef LIBRARYINTERNALDATABASE_H
#define LIBRARYINTERNALDATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

namespace Library {

    class LibraryInternalDatabase {

        public:

            static const QString tableVersion;
            static const QString databaseName;
            static const QString tableName;
            static const QString tableCollectionMapping;
            static const QString tableCollections;

            LibraryInternalDatabase();

            ~LibraryInternalDatabase();

            QSqlDatabase &database();

            // opens the internal QSqlDatabase.
            void open();

            // Returns the schema verison.
            int version() const;

            // Returns the absolute path of the
            // underlying SQL database.
            QString filePath() const;

        private:
            QSqlDatabase db;

            // Creates the table.
            bool createSchema();

            // Populates the database with dummy entries for testing purposes.
            bool loadFixtures();

            QString mFilePath;
    };
}

#endif // LIBRARYINTERNALDATABASE_H
