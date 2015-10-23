#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include "frontendcommon.h"

#include "phxpaths.h"
#include "logging.h"

namespace Library {

    class SystemDatabase {
        public:

            static QSqlDatabase database();

            static void open();
            static void close();

        public slots:

        private:
            SystemDatabase();

    };

}

#endif // SYSTEMDATABASE_H
