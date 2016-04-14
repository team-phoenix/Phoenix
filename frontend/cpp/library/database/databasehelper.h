#pragma once

#include <QSqlDatabase>

namespace Library {
    QSqlDatabase getLibretroDatabase();
    QSqlDatabase getUserDatabase();
    bool createUserDatabaseSchema( QSqlDatabase db );
}
