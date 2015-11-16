#include "gamescancontrollerproxy.h"

GameScanControllerProxy::GameScanControllerProxy( QObject *parent )
    : GameScanController( parent ),
      mThread( QThread( this ) )
{
    mGameScanController.moveToThread( &mThread );

    connect( &mGameScanController, mGameScanController::scanFinished(), &mThread, QThread::quit() );

    mThread.setPriority( QThread::NormalPriority );
    mThread.start();
}

void GameScanControllerProxy::startScan()
{
    mGameScanController.startScan();
}

