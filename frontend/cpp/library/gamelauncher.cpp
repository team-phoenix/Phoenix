#include "gamelauncher.h"
#include "systemdatabase.h"
#include "logging.h"

#include <QStringBuilder>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>

GameLauncher::GameLauncher(QObject *parent) :
    QObject(parent)
{
}

const QString GameLauncher::getDefaultCore( const QString system ) {

    const static QString statement = QStringLiteral( "SELECT core FROM defaultCoresMap WHERE systemname = ?" );
    auto query = QSqlQuery( Library::SystemDatabase::database() );
    query.prepare( statement );
    query.addBindValue( system );

    if ( !query.exec() ) {
        qFatal( "Error in SystemDatabase, 'getDefaultCore()': %s", qPrintable( query.lastError().text() ) );
    }

    QString defaultCore;
    if ( query.first() ) {
        defaultCore = query.value( 0 ).toString();
    }

#if defined( Q_OS_LINUX )
    defaultCore = QStringLiteral( "/usr/lib/libretro/" ) % defaultCore % QStringLiteral( ".so" );
#endif
#if defined( Q_OS_WIN )
    defaultCore = QStringLiteral( "C:\\Program Files\\Libretro\\Cores\\" ) % defaultCore % QStringLiteral( ".dll" );
#endif

    return std::move( defaultCore );
}

bool GameLauncher::verifyGame( const QString system, const QString rom ) {
    bool stuffWorks = false;
    stuffWorks = QFile::exists( system );
    if ( !stuffWorks ) {
        qCWarning(phxLibrary) << system << " does not exist. Launch will fail...";
    }

    stuffWorks = QFile::exists( rom );
    if ( !stuffWorks ) {
        qCWarning(phxLibrary) << rom << " does not exist. Launch will fail...";
    }

    return stuffWorks;
}
