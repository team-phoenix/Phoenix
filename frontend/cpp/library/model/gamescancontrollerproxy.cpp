#include "gamescancontrollerproxy.h"

GameScanControllerProxy::GameScanControllerProxy( QObject *parent )
    : GameScanController( parent ),
      mGameScanController( new GameScanController() ),
      mThread( new QThread() ) {
    mGameScanController->moveToThread( mThread );

    connect( &mGameScanController, &GameScanController::scanFinished, &mThread, &QThread::quit );


    connect( QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [ = ]() {
    } );

    startThread( QThread::NormalPriority );
}

GameScanControllerProxy::~GameScanControllerProxy() {
    const bool waitForClose = true;
    // Hang thread until the scanner thread is finished unloading.
    quitThread( waitForClose );

    qDebug() << "Quitting " << Q_FUNC_INFO;
}

void GameScanControllerProxy::startThread( const QThread::Priority priority ) {
    mThread->start( priority );
}

void GameScanControllerProxy::quitThread( const bool waitForClose ) {
    if( mThread->isRunning() ) {
        mThread->requestInterruption();
        mThread->quit();

        if( waitForClose ) {
            mThread->wait();
        }
    }
}

int GameScanControllerProxy::progress() {
    return mGameScanController->progress();
}

void GameScanControllerProxy::startScan() {
    mGameScanController->startScan();
}

void GameScanControllerProxy::appendScanPath( const QString scanPath ) {
    QMetaObject::invokeMethod( &mGameScanController, "appendScanPath", Q_ARG( QString, scanPath ) );
}

