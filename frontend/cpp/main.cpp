#include "frontendcommon.h"

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
#include "backendloader.h"

#include "debughandler.h"

// Misc
#include "phxpaths.h"
#include "logging.h"
#include "version.h"

// SDL2/Qt main() conflict fix
// Use SDL2's SDL_main() instead of Qt's qMain()
// Make sure that SDL.h is included *after* all other includes in main.cpp
// The other half of the fix is in frontend.pro
// http://blog.debao.me/2013/07/link-confilict-between-sdl-and-qt-under-windows/
#if defined( Q_OS_WIN )
// #define main qMain
#undef main
#include "SDL.h"
#endif

using namespace Library;

int main( int argc, char *argv[] ) {
    // Set some environment variables here
    {
        // This is the most direct way to set the render loop type
        //qputenv( "QSG_RENDER_LOOP", "threaded" );
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
    // This is for Libretro cores on Windows
    QString path( qgetenv( "PATH" ) );
    QString correctedPath = QCoreApplication::applicationDirPath() + "/MinGW;" + path;
    qputenv( "PATH", correctedPath.toLocal8Bit() );
#endif

    // For release builds, write to a log file along with the console
#ifdef QT_NO_DEBUG

    const QString logFile = Library::PhxPaths::userDataLocation() % '/' % QStringLiteral( "Logs" ) % '/' %
                            QDateTime::currentDateTime().toString( QStringLiteral( "ddd MMM d yyyy - h mm ss AP" ) ) %
                            QStringLiteral( ".log" );

    DebugHandler::install( logFile, DebugHandler::messageLog );

#endif

    qDebug().noquote() << "Phoenix" << PHOENIX_VER_STR;

    // Parse command line args, store them here
    QVariantMap commandLineSource = parseCommandLine( app );

    // Figure out the right paths for the environment, and create user storage folders if not already there
    Library::PhxPaths::initPaths();

    QThread::currentThread()->setObjectName( "Main/QML thread " );

    // The engine that runs our QML-based UI
    QQmlApplicationEngine engine;

    // Init the backend
    BackendPlugin plugin;
    plugin.registerTypes( "Phoenix.Backend" );
    Q_INIT_RESOURCE( backend_input_controllerdb );
    backendInit( argc, argv );

    // Set up the QML import paths
    // Set up the plugin directory path
    engine.addImportPath( app.applicationDirPath() + "/QML" );
#if defined( Q_OS_LINUX )
    engine.addImportPath( QStringLiteral( "/usr/lib/phoenix/QML" ) );
#endif

    // Give the QML engine our command line args
    engine.rootContext()->setContextProperty( "commandLineSource", commandLineSource );

    // Necessary to quit properly from QML
    QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QGuiApplication::quit );

    // Set application metadata
    QGuiApplication::setApplicationDisplayName( "Phoenix" );
    QGuiApplication::setApplicationName( "Phoenix" );
    QGuiApplication::setApplicationVersion( PHOENIX_VER_STR );
    QGuiApplication::setOrganizationName( "Team Phoenix" );
    QGuiApplication::setOrganizationDomain( "phoenix.vg" );

    qRegisterMetaType<Library::FileEntry>( "FileEntry" );

    // Register our custom QML-accessable/instantiable objects
    qmlRegisterType<SqlModel>( "Phoenix.Models", 1, 0, "SqlModel" );
    qmlRegisterType<SqlColumn>( "Phoenix.Models", 1, 0, "SqlColumn" );
    qmlRegisterType<SqlThreadedModel>( "Phoenix.Models", 1, 0, "SqlThreadedModel" );
    qmlRegisterType<DatabaseSettings>();
    qRegisterMetaType<SqlModel::FilterType>( "SqlModel::FilterType" );
    qRegisterMetaType<SqlModel::OrderBy>( "SqlModel::OrderBy" );

    qmlRegisterType<Library::CoreModel>( "Phoenix.Models", 1, 0, "CoreModel" );
    qmlRegisterType<Library::ImageCacher>( "vg.Phoenix.Cache", 1, 0, "ImageCacher" );
    qmlRegisterType<GameLauncher>( "Phoenix.Launcher", 1, 0, "GameLauncher" );

    qmlRegisterSingletonType<Library::PhxPaths>(
        "Phoenix.Paths", 1, 0, "PhxPaths", PhxPathsSingletonProviderCallback );
    qmlRegisterSingletonType<GameHasherController>(
        "Phoenix.Scanner", 1, 0, "GameHasherController", GameHasherControllerSingletonProviderCallback );

    // Register our game scanner types
    qRegisterMetaType<Library::FileEntry>( "Library::FileEntry" );
    qRegisterMetaType<Library::FileEntry>( "FileEntry" );
    qRegisterMetaType<Library::FileList>( "Library::FileList" );
    qRegisterMetaType<Library::FileList>( "FileList" );
    qRegisterMetaType<Library::GameScannerResult>( "Library::GameScannerResult" );
    qRegisterMetaType<Library::GameScannerResult>( "GameScannerResult" );
    //qRegisterMetaType<Library::GameData>( "GameData" );

    // Load the root QML object and everything under it
    engine.load( QDir::toNativeSeparators( Library::PhxPaths::resourceLocation() + "/QML/Phoenix/Phoenix/" +
                                           commandLineSource[ "mainSrc" ].toString() ) );

    // Begin main event loop
    // Once it returns, write return code to the log file if in release mode

    int exec = app.exec();

#ifdef QT_NO_DEBUG
    qDebug() << "Returned" << exec;
    DebugHandler::archive();
#endif

    return exec;
}
