#include "gamehashercontroller.h"
#include "logging.h"

#include <QThread>

GameHasherController::GameHasherController( QObject *parent ) : QObject( parent ),
    gameHasher( new GameHasher() ),
    gameHasherThread( new QThread() ),
    mProgress( 0 ),
    mRunning( false ),
    mPaused( false ) {
    // Set up GameHasher
    gameHasher->setObjectName( "GameHasher" );
    gameHasher->moveToThread( gameHasherThread );

    connect( gameHasherThread, &QThread::finished, gameHasher, &QObject::deleteLater );
    connect( this, &GameHasherController::shutdownGameHasher, gameHasher, &GameHasher::shutdown );
    connect( gameHasher, &GameHasher::scanCompleted, this, &GameHasherController::scanCompleted );
    connect( gameHasher, &GameHasher::filesNeedAssignment, this, &GameHasherController::filesNeedAssignment );
    connect( gameHasher, &GameHasher::progressChanged, this, &GameHasherController::setProgress );
    connect( gameHasher, &GameHasher::running, this, &GameHasherController::setRunning );

    connect( gameHasher, &GameHasher::scanCompleted, this, [ = ]( FileList results ) {
        qCDebug( phxLibrary ) << "scanCompleted from GameHasher," << results.size();
    } );


    gameHasherThread->setObjectName( "Game hasher thread" );
    gameHasherThread->start();

    // Ensure gameHasher safely shuts down on user-invoked quit
    connect( QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [ = ]() {
        qDebug() << "";
        qCInfo( phxLibrary ) << ">>>>>>>> User requested app to close, shutting down GameHasher (waiting up to 30 seconds)...";
        qDebug() << "";

        // Shut down coreControl (calls gameHasherThread->exit() too)
        emit shutdownGameHasher();

        // Shut down thread, block until it finishes
        gameHasherThread->wait( 30 * 1000 );
        gameHasherThread->deleteLater();

        qDebug() << "";
        qCInfo( phxLibrary ) << ">>>>>>>> Fully unloaded!";
        qDebug() << "";
    } );
}

int GameHasherController::progress() const {
    return mProgress;
}

bool GameHasherController::running() const {
    return mRunning;
}

bool GameHasherController::paused() const {
    return mPaused;
}

void GameHasherController::setRunning( bool running ) {
    mRunning = running;
    emit runningChanged();
}

void GameHasherController::setPaused( bool paused ) {
    mPaused = paused;
    emit pausedChanged();
}

void GameHasherController::scanForGames( QList<QUrl> urls ) {
    QStringList paths;

    for( QUrl url : urls ) {
        paths.append( url.toLocalFile() );
    }

    qCDebug( phxLibrary ) << "Sending" << urls.size() << "paths to GameHasher";
    QMetaObject::invokeMethod( gameHasher, "addPaths", Qt::QueuedConnection, Q_ARG( QStringList, paths ) );
}

void GameHasherController::setProgress( int progress ) {
    mProgress = progress;
    emit progressChanged();
}

void GameHasherController::pause() {
    QMetaObject::invokeMethod( gameHasher, "pause" );
    setPaused( true );
}

void GameHasherController::resume() {
    QMetaObject::invokeMethod( gameHasher, "resume" );
    setPaused( false );
}

void GameHasherController::cancel() {
    QMetaObject::invokeMethod( gameHasher, "cancel" );
}

QObject *GameHasherControllerSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine ) {
    Q_UNUSED( scriptEngine )
    GameHasherController *gameHasherController = new GameHasherController( engine );
    return gameHasherController;
}
