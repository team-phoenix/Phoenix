#include "frontendcommon.h"

#ifdef Q_OS_WIN
#include <io.h>
#endif

// Library
#include "cmdlineargs.h"
#include "coremodel.h"
#include "gamehasher.h"
#include "gamehashercontroller.h"
#include "gamelauncher.h"
#include "imagecacher.h"
#include "librarytypes.h"
#include "sqlthreadedmodel.h"

#include "backendplugin.h"

#include "debughandler.h"

// Misc
#include "phxpaths.h"
#include "logging.h"

// SDL2/Qt main() conflict fix
// Use SDL2's SDL_main() instead of Qt's qMain()
// Make sure that SDL.h is included *after* all other includes in main.cpp
// The other half of the fix is in frontend.pro
// http://blog.debao.me/2013/07/link-confilict-between-sdl-and-qt-under-windows/

// #define main qMain
#undef main
#include "SDL.h"

using namespace Library;

// Version string helper
#define xstr(s) str(s)
#define str(s) #s

int main( int argc, char *argv[] ) {
    // Set some environment variables here
    {
        // This is the most direct way to set the render loop type
        qputenv( "QSG_RENDER_LOOP", "threaded" );
    }

    // Use a custom message printing setup
    {
        qSetMessagePattern( DebugHandler::messageFormat() );
        DebugHandler::install( DebugHandler::messageHandler );
    }

    // Runs the main thread's event loop and handles messages from the windowing system
    QGuiApplication app( argc, argv );

#if defined( Q_OS_WIN )
    // Add the MinGW directory to PATH
    QString path( getenv( "PATH" ) );
    path.prepend( "/MinGW;" );
    path.prepend( QCoreApplication::applicationDirPath() );
    path.prepend( "PATH=" );
    QByteArray pathByteArray = path.toLocal8Bit();
    putenv( pathByteArray.data() );
#endif

    // Parse command line args, store them here
    QVariantMap commandLineSource = parseCommandLine( app );

    // Figure out the right paths for the environment, and create user storage folders if not already there
    Library::PhxPaths::initPaths();

    // For release builds, write to a log file along with the console
#ifdef QT_NO_DEBUG

    const QString logFile = Library::PhxPaths::userDataLocation() % '/' % QStringLiteral( "Logs" ) % '/' %
                            QDateTime::currentDateTime().toString( QStringLiteral( "ddd MMM d yyyy - h mm ss AP" ) ) %
                            QStringLiteral( ".log" );

    DebugHandler::install( logFile, DebugHandler::messageLog );

#endif

    // Print the version once we've set up debug logging
    qDebug().noquote() << "Phoenix" << QStringLiteral( xstr( PHOENIX_VER_STR ) );

    QThread::currentThread()->setObjectName( "Main/QML thread " );

    // The engine that runs our QML-based UI
    QQmlApplicationEngine engine;

    // Load the backend plugin immediately if we're using static Qt or simply inform the QML engine where to find the shared
    // library and qmldir file if using the normal shared Qt
#if defined( PHXSTATIC )
    BackendPlugin plugin;
    plugin.registerTypes( "vg.phoenix.backend" );
    Q_INIT_RESOURCE( controllerdb );
#else
    // Set up the plugin directory path
    engine.addImportPath( app.applicationDirPath() + QStringLiteral( "/Plugins" ) );
#if defined( Q_OS_LINUX )
    engine.addImportPath( QStringLiteral( "/usr/lib/phoenix/Plugins" ) );
#endif
#endif

    // Give the QML engine our command line args
    engine.rootContext()->setContextProperty( "commandLineSource", commandLineSource );

    // Necessary to quit properly from QML
    QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QGuiApplication::quit );

    // Set application metadata
    QGuiApplication::setApplicationDisplayName( QStringLiteral( "Phoenix" ) );
    QGuiApplication::setApplicationName( QStringLiteral( "Phoenix" ) );
    QGuiApplication::setApplicationVersion( QStringLiteral( xstr( PHOENIX_VER_STR ) ) );
    QGuiApplication::setOrganizationName( QStringLiteral( "Team Phoenix" ) );
    QGuiApplication::setOrganizationDomain( QStringLiteral( "phoenix.vg" ) );

    qRegisterMetaType<Library::FileEntry>( "FileEntry" );

    // Register our custom QML-accessable/instantiable objects
    qmlRegisterType<SqlModel>( "vg.phoenix.models", 1, 0, "SqlModel" );
    qmlRegisterType<SqlColumn>( "vg.phoenix.models", 1, 0, "SqlColumn" );
    qmlRegisterType<SqlThreadedModel>( "vg.phoenix.models", 1, 0, "SqlThreadedModel" );
    qmlRegisterType<DatabaseSettings>();
    qRegisterMetaType<SqlModel::FilterType>( "SqlModel::FilterType" );
    qRegisterMetaType<SqlModel::OrderBy>( "SqlModel::OrderBy" );

    qmlRegisterType<Library::CoreModel>( "vg.phoenix.models", 1, 0, "CoreModel" );
    qmlRegisterType<Library::ImageCacher>( "vg.phoenix.cache", 1, 0, "ImageCacher" );
    qmlRegisterType<GameLauncher>( "vg.phoenix.launcher", 1, 0, "GameLauncher" );

    // Register our custom QML-accessable objects and instantiate them here
    qmlRegisterSingletonType( QUrl( "qrc:/PhxTheme.qml" ), "vg.phoenix.themes", 1, 0, "PhxTheme" );
    qmlRegisterSingletonType<Library::PhxPaths>( "vg.phoenix.paths", 1, 0, "PhxPaths", PhxPathsSingletonProviderCallback );
    qmlRegisterSingletonType<GameHasherController>( "vg.phoenix.scanner", 1, 0, "GameHasherController", GameHasherControllerSingletonProviderCallback );

    // Register our game scanner types
    qRegisterMetaType<Library::FileEntry>( "Library::FileEntry" );
    qRegisterMetaType<Library::FileEntry>( "FileEntry" );
    qRegisterMetaType<Library::FileList>( "Library::FileList" );
    qRegisterMetaType<Library::FileList>( "FileList" );
    qRegisterMetaType<Library::GameScannerResult>( "Library::GameScannerResult" );
    qRegisterMetaType<Library::GameScannerResult>( "GameScannerResult" );
    //qRegisterMetaType<Library::GameData>( "GameData" );

    // Load the root QML object and everything under it
    engine.load( QUrl( QStringLiteral( "qrc:/Phoenix/" )
                       + commandLineSource[ QStringLiteral( "mainSrc" ) ].toString() ) );

    // Begin main event loop
    // Once it returns, write return code to the log file if in release mode

    int exec = app.exec();

#ifdef QT_NO_DEBUG
    qDebug() << "Returned" << exec;
    DebugHandler::archive();
#endif

    return exec;
}
