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

        // Deserialize the provided game path (as stored in the database), extracting the file to a temp folder if
        // within an archive.
        static QString trimmedGame( QString game );

        // Deserialize the provided game path (as stored in the database)
        static QString trimmedGameNoExtract( QString game );
};

#endif // GAMELAUNCHER_H
