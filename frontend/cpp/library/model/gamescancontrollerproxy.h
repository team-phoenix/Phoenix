#ifndef GAMESCANCONTROLLERPROXY_H
#define GAMESCANCONTROLLERPROXY_H

#include "frontendcommon.h"

#include "gamescancontroller.h"

class GameScanControllerProxy : public GameScanController {
        Q_OBJECT
    public:
        explicit GameScanControllerProxy( QObject *parent = 0 );

        void startThread( const QThread::Priority priority );

        void quitThread( const bool waitForClose = false );

        ~GameScanControllerProxy();

        int progress() override;

    signals:
        void startThreadedScan();

    public slots:
        void startScan() override;
        void appendScanPath( const QString scanPath ) override;

    private:
        GameScanController *mGameScanController;
        QThread *mThread;
};

#endif // GAMESCANCONTROLLERPROXY_H
