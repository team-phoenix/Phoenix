#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "videoitem.h"
#include "inputmanager.h"
#include "librarymodel.h"
#include "libraryworker.h"
#include "imagecacher.h"
#include "platformsmodel.h"

#include <memory.h>

void phoenixDebugMessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg ) {

    // Change this QString to reflect the message you want to get a stack trace for
    if( QString( msg ).contains( QStringLiteral( "Timers cannot be stopped from another thread" ) ) ) {

        int breakPointOnThisLine( 0 );
        Q_UNUSED( breakPointOnThisLine );

    }

    QByteArray localMsg = msg.toLocal8Bit();

    switch( type ) {
        case QtDebugMsg:
            fprintf( stderr, "Debug: %s (%s:%u, %s)\n",
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
    }

}



int main( int argc, char *argv[] ) {

    QApplication app( argc, argv );

    QApplication::setApplicationDisplayName( QStringLiteral( "Phoenix" ) );
    QApplication::setApplicationName( QStringLiteral( "Phoenix" ) );
    QApplication::setApplicationVersion( QStringLiteral( "1.0" ) );
    QApplication::setOrganizationDomain( QStringLiteral( "http://phoenix.vg/" ) );

    QQmlApplicationEngine engine;

    // Necessary to quit properly
    // QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QApplication::quit );

    // Register my types!
    VideoItem::registerTypes();
    InputManager::registerTypes();


    qmlRegisterType<Library::PlatformsModel>( "vg.phoenix.models", 1, 0, "PlatformsModel" );
    qmlRegisterType<Library::LibraryModel>( "vg.phoenix.models", 1, 0, "LibraryModel" );
    qmlRegisterType<Library::ImageCacher>( "vg.phoenix.cache", 1, 0, "ImageCacher" );

    qRegisterMetaType<Library::GameData>( "GameData" );

    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    return app.exec();

}
