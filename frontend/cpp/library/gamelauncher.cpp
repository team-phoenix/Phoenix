#include "gamelauncher.h"

using namespace Library;

GameLauncher::GameLauncher( QObject *parent ) :
    QObject( parent ) {
}

const QString GameLauncher::getDefaultCore( const QString system ) {

    // Let the constructor run so it'll make sure default cores are set for any new systems that might not have had their
    // defaults written to the user database yet because the user has not opened that settings page
    // Ugly hack or good idea?
    CoreModel *model = new CoreModel();
    delete model;

    const static QString statement = QStringLiteral( "SELECT defaultCore FROM defaultCores WHERE system = ?" );
    auto query = QSqlQuery( UserDatabase::instance()->database() );
    query.prepare( statement );
    query.addBindValue( system );

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

QString GameLauncher::trimmedGame( QString game ) {

    if( game.startsWith( QStringLiteral( "file://" ) ) ) {
        game.remove( QStringLiteral( "file://" ) );
    } else if( game.startsWith( QStringLiteral( "cue://" ) ) ) {
        game.remove( QStringLiteral( "cue://" ) );
    } else if( game.startsWith( QStringLiteral( "zip://" ) ) ) {
        game.remove( QStringLiteral( "zip://" ) );
        auto nameList = game.split( Library::ArchiveFileInfo::delimiter() );

        auto baseDestName = nameList.at( 1 );

        game = QDir::tempPath() + "/" + baseDestName;

        qDebug() << "temp dir: " << game;
        qDebug() << "return: " << JlCompress::extractFile( nameList.first(), nameList.at( 1 ), game );

    }

    return game;
}
