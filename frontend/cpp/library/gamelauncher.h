#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include "frontendcommon.h"

#include "systemdatabase.h"
#include "logging.h"
#include "phxpaths.h"
#include "archivefileinfo.h"

class GameLauncher : public QObject {
        Q_OBJECT
    public:
        explicit GameLauncher( QObject *parent = 0 );

    public slots:
        const QString getDefaultCore( const QString system );

        bool verify( const QString system, QString rom );

        QString trimmedGame( QString game );


    private:


};

#endif // GAMELAUNCHER_H
