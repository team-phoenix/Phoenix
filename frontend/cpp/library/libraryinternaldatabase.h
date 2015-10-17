#ifndef LIBRARYINTERNALDATABASE_H
#define LIBRARYINTERNALDATABASE_H

#include "frontendcommon.h"

#include "logging.h"
#include "phxpaths.h"

namespace Library {

    class LibraryInternalDatabase {

        public:

            class LibraryDatabasePointer {
                public:
                    LibraryDatabasePointer( LibraryInternalDatabase *db )
                        : mPtr( db ) {

                    }

                    LibraryInternalDatabase *get() {
                        return mPtr;
                    }

                    ~LibraryDatabasePointer()  {
                        mPtr->close();
                        delete mPtr;
                    }

                private:
                    LibraryInternalDatabase *mPtr;
            };

            static const QString tableVersion;
            static const QString databaseName;
            static const QString tableName;
            static const QString tableCollectionMappings;
            static const QString tableCollections;

            static LibraryInternalDatabase *instance();

            QSqlDatabase &database();

            // opens the internal QSqlDatabase.
            void open();

            // closes the interal QSqlDatabase.
            void close();

            // Returns the schema verison.
            int version() const;

            // Returns the absolute path of the
            // underlying SQL database.
            QString filePath() const;

        private:
            QSqlDatabase db;

            LibraryInternalDatabase();
            LibraryInternalDatabase( LibraryInternalDatabase const & ) = delete;
            LibraryInternalDatabase operator =( LibraryInternalDatabase const & ) = delete;

            // Creates the table.
            bool createSchema();

            // Populates the database with dummy entries for testing purposes.
            bool loadFixtures();

            QString mFilePath;
    };
}

#endif // LIBRARYINTERNALDATABASE_H
