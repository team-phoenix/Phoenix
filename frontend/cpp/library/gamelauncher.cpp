#include "gamelauncher.h"
#include "coremodel.h"
#include "archivefile.h"

using namespace Library;

GameLauncher::GameLauncher( QObject *parent ) :
    QObject( parent ) {
}

const QString GameLauncher::getDefaultCore( const QString system ) {
    Q_UNUSED( system )

    // Let the constructor run so it'll make sure default cores are set for any new systems that might not have had their
    // defaults written to the user database yet because the user has not opened that settings page
    // Ugly hack or good idea?
    CoreModel *model = new CoreModel();
    delete model;

    QString connectionName = QThread::currentThread()->objectName() % QStringLiteral( "userdata" );
    QSqlDatabase userDatabase = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), connectionName );

    if( !userDatabase.isOpen() ) {
        userDatabase.setDatabaseName( PhxPaths::userDataLocation() % QStringLiteral( "/userdata.sqlite" ) );
        userDatabase.open();
        Q_ASSERT( userDatabase.isOpen() );
    }

    QSqlQuery query = QSqlQuery( userDatabase );
    query.prepare( QStringLiteral( "SELECT defaultCore FROM defaultCores WHERE system = :system" ) );
    query.bindValue( QStringLiteral( ":system" ), system );

    auto exec = query.exec();
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

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

    return defaultCore;
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

QString GameLauncher::trimmedGame( QString game ) {

    if( game.startsWith( QStringLiteral( "file://" ) ) ) {
        game.remove( QStringLiteral( "file://" ) );
    } else if( game.startsWith( QStringLiteral( "cue://" ) ) ) {
        game.remove( QStringLiteral( "cue://" ) );
    } else if( game.startsWith( QStringLiteral( "zip://" ) ) ) {
        game.remove( QStringLiteral( "zip://" ) );
        auto nameList = game.split( QStringLiteral( "|||" ) );

        auto baseDestName = nameList.at( 1 );

        game = QDir::tempPath() + "/" + baseDestName;

        qDebug() << "temp dir: " << game;
        qDebug() << "return: " << JlCompress::extractFile( nameList.first(), nameList.at( 1 ), game );

    }

    return game;
}

QString GameLauncher::trimmedGameNoExtract( QString game ) {

    if( game.startsWith( QStringLiteral( "file://" ) ) ) {
        game.remove( QStringLiteral( "file://" ) );
    } else if( game.startsWith( QStringLiteral( "cue://" ) ) ) {
        game.remove( QStringLiteral( "cue://" ) );
    } else if( game.startsWith( QStringLiteral( "zip://" ) ) ) {
        game.remove( QStringLiteral( "zip://" ) );
        auto nameList = game.split( QStringLiteral( "|||" ) );

        auto baseDestName = nameList.at( 1 );

        game = QDir::tempPath() + "/" + baseDestName;
    }

    return game;
}
