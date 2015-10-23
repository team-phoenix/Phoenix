#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include "frontendcommon.h"

#include "phxpaths.h"
#include "logging.h"

namespace Library {

    class LibretroDatabase {
        public:

            static QSqlDatabase database();

            static void open();
            static void close();

        public slots:

        private:
            LibretroDatabase();

    };

}

#endif // SYSTEMDATABASE_H
