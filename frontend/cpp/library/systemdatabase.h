#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include <QSqlDatabase>
#include <QObject>

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
