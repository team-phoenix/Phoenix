#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include "frontendcommon.h"

#include "systemdatabase.h"
#include "logging.h"
#include "phxpaths.h"

class GameLauncher : public QObject {
        Q_OBJECT
    public:
        explicit GameLauncher( QObject *parent = 0 );

    public slots:
        const QString getDefaultCore( const QString system );

        bool verifyGame( const QString system, const QString rom );

};

#endif // GAMELAUNCHER_H
