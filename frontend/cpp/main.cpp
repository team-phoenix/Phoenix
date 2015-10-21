#include "frontendcommon.h"

#include "videoitem.h"
#include "inputmanager.h"
#include "librarymodel.h"
#include "libraryworker.h"
#include "imagecacher.h"
#include "platformsmodel.h"
#include "collectionsmodel.h"
#include "phxpaths.h"
#include "systemdatabase.h"
#include "metadatadatabase.h"
#include "defaultcoremodel.h"
#include "gamelauncher.h"

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

    QApplication app( argc, argv );

    // On Windows, the organization domain is used to set the registry key path... for some reason
    QApplication::setApplicationDisplayName( QStringLiteral( "Phoenix" ) );
    QApplication::setApplicationName( QStringLiteral( "Phoenix" ) );
    QApplication::setApplicationVersion( QStringLiteral( "1.0" ) );
    QApplication::setOrganizationName( QStringLiteral( "Team Phoenix" ) );
    QApplication::setOrganizationDomain( QStringLiteral( "phoenix.vg" ) );

    // Create the folders used by Phoenix.
    Library::PhxPaths::createAllPaths();

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
    Library::SystemDatabase::open();
    Library::MetaDataDatabase::open();

    QQmlApplicationEngine engine;

    // Necessary to quit properly
    QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QApplication::quit );

    // Register my types!
    VideoItem::registerTypes();
    InputManager::registerTypes();

    qmlRegisterSingletonType( QUrl( "qrc:/PhxTheme.qml" ), "vg.phoenix.themes", 1, 0, "PhxTheme" );

    qmlRegisterType<Library::PlatformsModel>( "vg.phoenix.models", 1, 0, "PlatformsModel" );
    qmlRegisterType<Library::CollectionsModel>( "vg.phoenix.models", 1, 0, "CollectionsModel" );
    qmlRegisterType<Library::LibraryModel>( "vg.phoenix.models", 1, 0, "LibraryModel" );
    qmlRegisterType<Library::DefaultCoreModel>( "vg.phoenix.models", 1, 0, "DefaultCoreModel" );
    qmlRegisterType<Library::ImageCacher>( "vg.phoenix.cache", 1, 0, "ImageCacher" );
    qmlRegisterSingletonType<Library::PhxPaths>( "vg.phoenix.paths", 1, 0, "PhxPaths", PhxPathsSingletonProviderCallback );
    qmlRegisterType<GameLauncher>( "vg.phoenix.launcher", 1, 0, "GameLauncher" );

    qRegisterMetaType<Library::GameData>( "GameData" );

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

    // Run the app and write return code if in release mode
#ifdef QT_NO_DEBUG
    int ret = app.exec();
    fprintf( logFP, "Returned %d", ret );
    fclose( logFP );
    return ret;
#else
    return app.exec();
#endif

}
