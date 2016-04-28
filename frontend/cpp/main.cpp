#include "frontendcommon.h"

// Library
#include "gamelauncher.h"
#include "imagecacher.h"
#include "coremodel.h"
#include "librarytypes.h"
#include "sqlthreadedmodel.h"
#include "gamehasher.h"
#include "gamehashercontroller.h"

// Backend
#include "cmdlineargs.h"
#include "consumer.h"
#include "control.h"
#include "controllable.h"
#include "core.h"
#include "gameconsole.h"
#include "globalgamepad.h"
#include "inputmanager.h"
#include "libretrocore.h"
#include "controloutput.h"
#include "producer.h"
#include "videooutput.h"
#include "videooutputnode.h"

// Misc
#include "logging.h"
#include "phxpaths.h"
#include "controlhelper.h"
#include "debughelper.h"

using namespace Library;

// Version string helper
#define xstr(s) str(s)
#define str(s) #s

int main( int argc, char *argv[] ) {
    // Init controller db file for backend
    Q_INIT_RESOURCE( controllerdb );

    // Uncomment this to enable the message handler for debugging and stack tracing
    // qInstallMessageHandler( phoenixDebugMessageHandler );

    QThread::currentThread()->setObjectName( "Main/QML thread " );

    // Handles stuff with the windowing system
    QGuiApplication app( argc, argv );

    QVariantMap commandLineSource = parseCommandLine( app );

    // The engine that runs our QML-based UI
    QQmlApplicationEngine engine;

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

    // Register our custom types for use within QML
    qmlRegisterType<VideoOutputNode>( "vg.phoenix.backend", 1, 0, "VideoOutputNode" );
    qmlRegisterType<VideoOutput>( "vg.phoenix.backend", 1, 0, "VideoOutput" );
    qmlRegisterType<ControlOutput>( "vg.phoenix.backend", 1, 0, "ControlOutput" );
    qmlRegisterType<GlobalGamepad>( "vg.phoenix.backend", 1, 0, "GlobalGamepad" );
    qmlRegisterType<GameConsole>( "vg.phoenix.backend", 1, 0, "CoreControl" );
    qmlRegisterUncreatableType<ControlHelper>( "vg.phoenix.backend", 1, 0, "Control", "Control or its subclasses cannot be instantiated from QML." );
    InputManager::registerTypes();

    // Needed for connecting signals/slots
    qRegisterMetaType<Control::State>( "Control::State" );
    qRegisterMetaType<ControlHelper::State>( "ControlHelper::State" );
    qRegisterMetaType<Node::Command>( "Command" );
    qRegisterMetaType<Node::DataType>( "DataType" );
    qRegisterMetaType<Node::State>( "State" );
    qmlRegisterUncreatableType<Node>( "vg.phoenix.backend", 1, 0, "Node", "Node or its subclasses cannot be instantiated from QML." );
    qRegisterMetaType<size_t>( "size_t" );
    qRegisterMetaType<QStringMap>();
    qRegisterMetaType<ProducerFormat>();

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

    // Give the QML engine our command line args
    engine.rootContext()->setContextProperty( "commandLineSource", commandLineSource );

    // Load the root QML object and everything under it
    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    // Ensure custom controller DB file exists
    QFile gameControllerDBFile( Library::PhxPaths::userDataLocation() % '/' % QStringLiteral( "gamecontrollerdb.txt" ) );

    if( !gameControllerDBFile.exists() ) {
        gameControllerDBFile.open( QIODevice::ReadWrite );
        QTextStream stream( &gameControllerDBFile );
        stream << "# Insert your custom definitions here" << endl;
    }

    // Set InputManager's custom controller DB file
    QQmlProperty prop( engine.rootObjects().first(), "inputManager.controllerDBFile" );
    Q_ASSERT( prop.isValid() );
    QString path = Library::PhxPaths::userDataLocation() % QStringLiteral( "/gamecontrollerdb.txt" );
    QVariant pathVar( path );
    prop.write( pathVar );

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
