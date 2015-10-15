#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include "frontendcommon.h"

#include "systemdatabase.h"
#include "logging.h"
#include "phxpaths.h"
#include "archievefileinfo.h"
#include "JlCompress.h"
#include "QFileInfo"

class GameLauncher : public QObject {
        Q_OBJECT
    public:
        explicit GameLauncher( QObject *parent = 0 );

    public slots:
        const QString getDefaultCore( const QString system );

        bool verify( const QString system, QString rom );

        QString trimmedGame( QString game ) {

            if ( game.startsWith( QStringLiteral( "file://" ) ) ) {
                game.remove( QStringLiteral( "file://" ) );
            } else if ( game.startsWith( QStringLiteral( "cue://" ) ) ) {
                game.remove( QStringLiteral( "cue://" ) );
            } else if ( game.startsWith( QStringLiteral( "zip://" ) ) ) {
                game.remove( QStringLiteral( "zip://" ) );
                auto nameList = game.split( Library::ArchieveFileInfo::delimiter() );

                auto baseDestName = nameList.at( 1 );

                game = QFileInfo( nameList.at( 0 ) ).canonicalPath() + "/" + baseDestName;
                JlCompress::extractFile( nameList.first(), nameList.at( 1 ), game );

            }

            return game;
        }


    private:


};

#endif // GAMELAUNCHER_H
