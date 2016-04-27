#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include "frontendcommon.h"

#include "logging.h"
#include "phxpaths.h"

class GameLauncher : public QObject {
        Q_OBJECT
    public:
        explicit GameLauncher( QObject *parent = 0 );

    public slots:
        static const QString getDefaultCore( const QString system );

        static bool verify( const QString system, QString rom );

        static QString trimmedGame( QString game );
        static QString trimmedGameNoExtract( QString game );
};

#endif // GAMELAUNCHER_H
