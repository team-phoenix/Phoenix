#include "libretromodel.h"

#include "libretrodatabase.h"

using namespace Library;

LibretroModel::LibretroModel()
{
    LibretroDatabase database;

    QSqlQuery query( database );

    bool exec = query.exec( QStringLiteral( "SELECT DISTINCT UUID, defaultCore, friendlyName, manufacturer FROM system "
                                          "WHERE enabled = 1" ) );
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

    // Grab the system list from the system DB along with the default core
    while( query.next() ) {
        QString system = query.value( 0 ).toString();
        QString defaultCore = query.value( 1 ).toString();
        QString friendlyName = query.value( 2 ).toString();
        QString manufacturer = query.value( 3 ).toString();

        qDebug() << system;
        qDebug() << defaultCore;
        qDebug() << friendlyName;
        qDebug() << manufacturer;

    }
}

