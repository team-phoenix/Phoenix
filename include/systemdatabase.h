#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include <QSqlDatabase>
#include "logging.h"

class SystemDatabase {
public:
    SystemDatabase();
    ~SystemDatabase();

    QSqlDatabase &handle();
    void open();
    bool valid();

private:
    QSqlDatabase db;
};

#endif // SYSTEMDATABASE_H
