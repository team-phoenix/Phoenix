#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include "frontendcommon.h"
#include "phxpaths.h"
#include "logging.h"

#include <QMutex>

/*
 *
 * All Database classes will folow this interface.
 * class Base
    {
    public:
        Base() = delete;

        enum ThreadMode {
            NeedsMutex,
            NoMutex,
        };

        static QSqlDatabase database() = 0;
        static void open(  ) = 0;
        static void close() = 0;
        static void addDatabase() = 0;
        static void removeDatabase() = 0;
    };
 *
 *
 */

namespace Library {

    class LibretroDatabase {
        public:

            enum ThreadMode {
                NeedsMutex,
                NoMutex,
            };

            LibretroDatabase();

            LibretroDatabase( const ThreadMode mode );

            ~LibretroDatabase();

            //operator QSqlDatabase() const
           // {
            //    return database();
           // }

            explicit operator QSqlQuery() {
                return QSqlQuery( database() );
            }

            QSqlDatabase database() const;

            void open( const ThreadMode mode = NoMutex );
            void close( const ThreadMode mode = NoMutex );

            static void addDatabase();
            static void removeDatabase();

            static QMutex mutex;

    private:
            ThreadMode mMode;
    };

}

#endif // SYSTEMDATABASE_H
