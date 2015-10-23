#ifndef USERDATABASE_H
#define USERDATABASE_H

#include "frontendcommon.h"

#include "logging.h"
#include "phxpaths.h"

namespace Library {

    class UserDatabase {

        public:

            class UserDatabasePointer {
                public:
                    UserDatabasePointer( UserDatabase *db )
                        : mPtr( db ) {

                    }

                    UserDatabase *get() {
                        return mPtr;
                    }

                    ~UserDatabasePointer()  {
                        mPtr->close();
                        delete mPtr;
                    }

                private:
                    UserDatabase *mPtr;
            };

            static const QString tableVersion;
            static const QString databaseName;
            static const QString tableName;
            static const QString tableCollectionMappings;
            static const QString tableCollections;
            static const QString tableDefaultCores;

            static UserDatabase *instance();

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

            UserDatabase();
            UserDatabase( UserDatabase const & ) = delete;
            UserDatabase operator =( UserDatabase const & ) = delete;

            // Creates the table.
            bool createSchema();

            // Populates the database with dummy entries for testing purposes.
            bool loadFixtures();

            QString mFilePath;
    };
}

#endif // USERDATABASE_H
