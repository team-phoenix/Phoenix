#include "systemdatabase.h"

using namespace Library;

QSqlDatabase SystemDatabase::database() {
    return QSqlDatabase::database( QStringLiteral( "SYSTEMS" ) );
}

void SystemDatabase::close() {
    if( QSqlDatabase::contains( QStringLiteral( "SYSTEMS" ) ) ) {
        qDebug( phxLibrary ) << "closing SYSTEM database";
        auto db = QSqlDatabase::database( QStringLiteral( "SYSTEMS" ) );
        db.close();
        QSqlDatabase::removeDatabase( QStringLiteral( "SYSTEMS" ) );
    }
}

void SystemDatabase::open() {
    auto db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), QStringLiteral( "SYSTEMS" ) );

    QString dataPathStr = PhxPaths::metadataLocation();
    Q_ASSERT_X( !dataPathStr.isEmpty(), Q_FUNC_INFO, "dataPathStr.isEmpty()" );

    QDir dataPath( dataPathStr );

    auto databaseName = QStringLiteral( "libretro.sqlite" );
    auto filePath = dataPath.filePath( databaseName );

    db.setDatabaseName( filePath );

    auto open = db.open();
    Q_ASSERT_X( open, Q_FUNC_INFO, "db.open() == false") ;

    qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );
}
