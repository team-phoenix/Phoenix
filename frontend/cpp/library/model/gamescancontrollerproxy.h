#ifndef GAMESCANCONTROLLERPROXY_H
#define GAMESCANCONTROLLERPROXY_H

#include "gamescancontroller.h"

#include <QThread>

class GameScanControllerProxy : public GameScanController
{
    Q_OBJECT
public:
    explicit GameScanControllerProxy( QObject *parent = 0 );

signals:
    void startThreadedScan();

public slots:

private:
    GameScanController mGameScanController;
    QThread mThread;
};

#endif // GAMESCANCONTROLLERPROXY_H
