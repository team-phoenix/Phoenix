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

// Misc
#include "debughelper.h"
#include "phxpaths.h"
#include "logging.h"


using namespace Library;

// Version string helper
#define xstr(s) str(s)
#define str(s) #s

int main( int argc, char *argv[] ) {
    // This is the most direct way to set the render loop type
    putenv( const_cast<char *>( "QSG_RENDER_LOOP=threaded" ) );

    // Uncomment this to enable the message handler for debugging and stack tracing
    // qInstallMessageHandler( phoenixDebugMessageHandler );

    QThread::currentThread()->setObjectName( "Main/QML thread " );

    // Handles stuff with the windowing system
    QGuiApplication app( argc, argv );

    // The engine that runs our QML-based UI
    QQmlApplicationEngine engine;

    // Set up the plugin directory path
    engine.addImportPath( app.applicationDirPath() + QStringLiteral( "/Plugins" ) );

    // Parse command line args, store them here
    QVariantMap commandLineSource = parseCommandLine( app );

    // Give the QML engine our command line args
    engine.rootContext()->setContextProperty( "commandLineSource", commandLineSource );

    // Set application metadata
    QGuiApplication::setApplicationDisplayName( QStringLiteral( "Phoenix" ) );
    QGuiApplication::setApplicationName( QStringLiteral( "Phoenix" ) );
    QGuiApplication::setApplicationVersion( QStringLiteral( xstr( PHOENIX_VERSION_STR ) ) );
    QGuiApplication::setOrganizationName( QStringLiteral( "Team Phoenix" ) );
    QGuiApplication::setOrganizationDomain( QStringLiteral( "phoenix.vg" ) );

    // Figure out the right paths for the environment, and create user storage folders if not already there
    Library::PhxPaths::initPaths();

    // For release builds, write to a log file along with the console
#ifdef QT_NO_DEBUG
    QFile logFile( Library::PhxPaths::userDataLocation() % '/' % QStringLiteral( "Logs" ) % '/' %
                   QDateTime::currentDateTime().toString( QStringLiteral( "ddd MMM d yyyy - h mm ss AP" ) ) %
                   QStringLiteral( ".log" ) );

    // If this fails... how would we know? :)
    logFile.open( QIODevice::WriteOnly | QIODevice::Text );
    int logFD = logFile.handle();
    logFP = fdopen( dup( logFD ), "w" );
    qInstallMessageHandler( phoenixDebugMessageLog );
#endif

    // Necessary to quit properly from QML
    QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QGuiApplication::quit );

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
    engine.load( QUrl( QStringLiteral( "qrc:/main/" )
                       + commandLineSource[ QStringLiteral( "mainSrc" ) ].toString() ) );

    // Ensure custom controller DB file exists
    // FIXME: Use with GamepadManager
    // QFile gameControllerDBFile( Library::PhxPaths::userDataLocation() % '/' % QStringLiteral( "gamecontrollerdb.txt" ) );
    //
    // if( !gameControllerDBFile.exists() ) {
    //     gameControllerDBFile.open( QIODevice::ReadWrite );
    //     QTextStream stream( &gameControllerDBFile );
    //     stream << "# Insert your custom definitions here" << endl;
    // }

    // Set InputManager's custom controller DB file
    // FIXME: Use with GamepadManager
    // QQmlProperty prop( engine.rootObjects().first(), "inputManager.controllerDBFile" );
    // Q_ASSERT( prop.isValid() );
    // QString path = Library::PhxPaths::userDataLocation() % QStringLiteral( "/gamecontrollerdb.txt" );
    // QVariant pathVar( path );
    // prop.write( pathVar );

    // Run the app and write return code to the log file if in release mode
#ifdef QT_NO_DEBUG
    int ret = app.exec();
    fprintf( logFP, "Returned %d\n", ret );
    fclose( logFP );
    return ret;
#else
    // Otherwise, just run it normally
    return app.exec();
#endif
}
