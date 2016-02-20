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


            LibretroDatabase() = delete;

            static QSqlDatabase database();
            static void open( const ThreadMode mode = NoMutex );
            static void close( const ThreadMode mode = NoMutex );
            static addDatabase();
            static removeDatabase();

            static QMutex mutex;
    };

}

#endif // SYSTEMDATABASE_H
