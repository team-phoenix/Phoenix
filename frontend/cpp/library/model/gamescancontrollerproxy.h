#ifndef GAMESCANCONTROLLERPROXY_H
#define GAMESCANCONTROLLERPROXY_H

#include "frontendcommon.h"

#include "gamescancontroller.h"

class GameScanControllerProxy {
        Q_OBJECT

    public:
        explicit GameScanControllerProxy( QObject *parent = 0 );

        void startThread( const QThread::Priority priority );

        void quitThread( const bool waitForClose = false );

        ~GameScanControllerProxy();

        int progress();

    signals:
        void startThreadedScan();

    public slots:
        void startScan();
        void appendScanPath( const QString scanPath );

    private:
        GameScanController *mGameScanController;
        QThread *mThread;
};

#endif // GAMESCANCONTROLLERPROXY_H
