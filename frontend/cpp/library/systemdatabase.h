#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include <QtCore>
#include <QtSql>

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
