#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include <QSqlDatabase>

namespace Library {

    class SystemDatabase {
    public:
        static QSqlDatabase database();

        static void open();
        static void close();

    private:
        SystemDatabase();

    };

}

#endif // SYSTEMDATABASE_H
