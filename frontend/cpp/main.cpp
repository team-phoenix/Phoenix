#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "videoitem.h"
#include "inputmanager.h"
#include "librarymodel.h"
#include "libraryworker.h"
#include "imagecacher.h"
#include "platformsmodel.h"
#include "collectionsmodel.h"
#include "platform.h"
#include "phxpaths.h"

#include <memory.h>

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

int main( int argc, char *argv[] ) {

    // Uncomment this to enable the message handler for debugging and stack tracing
    // qInstallMessageHandler( phoenixDebugMessageHandler );

    QApplication app( argc, argv );

    // On Windows, the organization domain is used to set the registry key path... for some reason
    QApplication::setApplicationDisplayName( QStringLiteral( "Phoenix" ) );
    QApplication::setApplicationName( QStringLiteral( "Phoenix" ) );
    QApplication::setApplicationVersion( QStringLiteral( "1.0" ) );
    QApplication::setOrganizationDomain( QStringLiteral( "phoenix.vg" ) );

    Library::PhxPaths::CreateAllPaths();

    QQmlApplicationEngine engine;

    // Necessary to quit properly
    // QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QApplication::quit );

    // Register my types!
    VideoItem::registerTypes();
    InputManager::registerTypes();

    qmlRegisterSingletonType( QUrl( "qrc:/PhxTheme.qml" ), "vg.phoenix.themes", 1, 0, "PhxTheme" );
    qmlRegisterType<Library::PlatformsModel>( "vg.phoenix.models", 1, 0, "PlatformsModel" );
    qmlRegisterType<Library::CollectionsModel>( "vg.phoenix.models", 1, 0, "CollectionsModel" );
    qmlRegisterType<Library::LibraryModel>( "vg.phoenix.models", 1, 0, "LibraryModel" );
    qmlRegisterType<Library::ImageCacher>( "vg.phoenix.cache", 1, 0, "ImageCacher" );
    qmlRegisterType<Library::Platform>( "vg.phoenix.models", 1, 0, "Platform" );

    qRegisterMetaType<Library::GameData>( "GameData" );

    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    return app.exec();

}
