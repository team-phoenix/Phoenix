#include "frontendcommon.h"

// Library
#include "gamelauncher.h"
#include "gamescanner.h"
#include "imagecacher.h"
#include "metadatadatabase.h"
#include "libretrodatabase.h"
#include "collectionsmodel.h"
#include "librarymodel.h"
#include "coremodel.h"
#include "platformsmodel.h"

// Backend
#include "control.h"
#include "controllable.h"
#include "producer.h"
#include "consumer.h"
#include "corecontrolproxy.h"
#include "corecontrol.h"
#include "inputmanager.h"
#include "core.h"
#include "libretrocore.h"
#include "videooutput.h"

// Misc
#include "logging.h"
#include "phxpaths.h"
#include "controlhelper.h"

// This is used to get the stack trace behind whatever debug message you want to diagnose
// Simply change the message string below to whatever you want (partial string matching), set the breakpoint
// and uncomment the first line in main()
void phoenixDebugMessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg ) {

    // Change this QString to reflect the message you want to get a stack trace for
    if( QString( msg ).contains( QStringLiteral( "YOUR MESSAGE HERE" ) ) ) {

        // Put a breakpoint over this line...
        int breakPointOnThisLine( 0 );

        // ...and not this one
        Q_UNUSED( breakPointOnThisLine );

    }

    QByteArray localMsg = msg.toLocal8Bit();

    switch( type ) {

        case QtDebugMsg:
            fprintf( stderr, "Debug: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtInfoMsg:
            fprintf( stderr, "Info: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtWarningMsg:
            fprintf( stderr, "Warning: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtCriticalMsg:
            fprintf( stderr, "Critical: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtFatalMsg:
            fprintf( stderr, "Fatal: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            abort();
            break;

        default:
            break;

    }

}

FILE *logFP = nullptr;

// Alternate version, writes to file
void phoenixDebugMessageLog( QtMsgType type, const QMessageLogContext &context, const QString &msg ) {

    QByteArray localMsg = msg.toLocal8Bit();

    switch( type ) {

        case QtDebugMsg:
            fprintf( logFP, "Debug: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtInfoMsg:
            fprintf( logFP, "Info: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtWarningMsg:
            fprintf( logFP, "Warning: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtCriticalMsg:
            fprintf( logFP, "Critical: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            break;

        case QtFatalMsg:
            fprintf( logFP, "Fatal: %s (%s:%u, %s)\n",
                     localMsg.constData(), context.file, context.line, context.function );
            abort();
            break;

        default:
            break;

    }

    // Print to console too, just in case
    phoenixDebugMessageHandler( type, context, msg );

}

int main( int argc, char *argv[] ) {

    // Init controller db file for backend
    Q_INIT_RESOURCE( controllerdb );

    // Uncomment this to enable the message handler for debugging and stack tracing
    // qInstallMessageHandler( phoenixDebugMessageHandler );

    QThread::currentThread()->setObjectName( "Main thread" );

    // Handles stuff with the windowing system
    QGuiApplication app( argc, argv );

    // The engine that runs our QML-based UI
    QQmlApplicationEngine engine;

    // Set application metadata
    QGuiApplication::setApplicationDisplayName( QStringLiteral( "Phoenix" ) );
    QGuiApplication::setApplicationName( QStringLiteral( "Phoenix" ) );
    QGuiApplication::setApplicationVersion( QStringLiteral( "0.0.1" ) );
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

    // Open connections to the SQL databases.
    Library::LibretroDatabase::open();
    Library::MetaDataDatabase::open();

    // Necessary to quit properly
    QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QGuiApplication::quit );

    // Register our custom types for use within QML
    // VideoItem::registerTypes();
    qmlRegisterType<VideoOutput>( "vg.phoenix.backend", 1, 0, "VideoOutput" );
    qmlRegisterType<CoreControlProxy>( "vg.phoenix.backend", 1, 0, "CoreControl" );
    qmlRegisterUncreatableType<ControlHelper>( "vg.phoenix.backend", 1, 0, "Control", "Control or its subclasses cannot be instantiated from QML." );
    InputManager::registerTypes();

    // Needed for connecting signals/slots
    qRegisterMetaType<Control::State>( "Control::State" );
    qRegisterMetaType<ControlHelper::State>( "ControlHelper::State" );
    qRegisterMetaType<size_t>( "size_t" );
    qRegisterMetaType<QStringMap>();
    qRegisterMetaType<ProducerFormat>();

    // Register our custom QML-accessable/instantiable objects
    qmlRegisterType<Library::PlatformsModel>( "vg.phoenix.models", 1, 0, "PlatformsModel" );
    qmlRegisterType<Library::CollectionsModel>( "vg.phoenix.models", 1, 0, "CollectionsModel" );
    qmlRegisterType<Library::LibraryModel>( "vg.phoenix.models", 1, 0, "LibraryModel" );
    qmlRegisterType<Library::CoreModel>( "vg.phoenix.models", 1, 0, "CoreModel" );
    qmlRegisterType<Library::ImageCacher>( "vg.phoenix.cache", 1, 0, "ImageCacher" );
    qmlRegisterType<GameLauncher>( "vg.phoenix.launcher", 1, 0, "GameLauncher" );

    // Register our custom QML-accessable objects and instantiate them here
    qmlRegisterSingletonType( QUrl( "qrc:/PhxTheme.qml" ), "vg.phoenix.themes", 1, 0, "PhxTheme" );
    qmlRegisterSingletonType<Library::PhxPaths>( "vg.phoenix.paths", 1, 0, "PhxPaths", PhxPathsSingletonProviderCallback );

    qRegisterMetaType<Library::GameData>( "GameData" );

    // Load the root QML object and everything under it
    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    // Ensure custom controller DB file exists
    QFile gameControllerDBFile( Library::PhxPaths::userDataLocation() % '/' % QStringLiteral( "gamecontrollerdb.txt" ) );

    if( !gameControllerDBFile.exists() ) {
        gameControllerDBFile.open( QIODevice::ReadWrite );
        QTextStream stream( &gameControllerDBFile );
        stream << "# Insert your custom definitions here" << endl;
        gameControllerDBFile.close();
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
    fprintf( logFP, "Returned %d", ret );
    fclose( logFP );
    return ret;
#else
    // Otherwise, just run it normally
    return app.exec();
#endif

}
