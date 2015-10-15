#include "gamelauncher.h"

using namespace Library;

GameLauncher::GameLauncher( QObject *parent ) :
    QObject( parent ) {
}

const QString GameLauncher::getDefaultCore( const QString system ) {

    const static QString statement = QStringLiteral( "SELECT core FROM defaultCoresMap WHERE systemname = ?" );
    auto query = QSqlQuery( Library::SystemDatabase::database() );
    query.prepare( statement );
    query.addBindValue( system );

    if( !query.exec() ) {
        qFatal( "Error in SystemDatabase, 'getDefaultCore()': %s", qPrintable( query.lastError().text() ) );
    }

    QString defaultCore;

    if( query.first() ) {
        defaultCore = query.value( 0 ).toString();
    }

#if defined( Q_OS_WIN )
    defaultCore = PhxPaths::coreLocation() % QStringLiteral( "/" ) % defaultCore % QStringLiteral( ".dll" );
#endif
#if defined( Q_OS_MAC )
    defaultCore = PhxPaths::coreLocation() % QStringLiteral( "/" ) % defaultCore % QStringLiteral( ".dylib" );
#endif
#if defined( Q_OS_LINUX )
    defaultCore = PhxPaths::coreLocation() % QStringLiteral( "/" ) % defaultCore % QStringLiteral( ".so" );
#endif

    return std::move( defaultCore );
}

bool GameLauncher::verify( const QString system, QString rom ) {

    bool coreExists = QFile::exists( system );
    if( !coreExists ) {
        qCWarning( phxLibrary ) << system << " does not exist. Launch will fail...";
    }

    bool gameExists = QFile::exists( rom );
    if( !gameExists ) {
        qCWarning( phxLibrary ) << rom << " does not exist. Launch will fail...";
    }

    return coreExists && gameExists;
}
