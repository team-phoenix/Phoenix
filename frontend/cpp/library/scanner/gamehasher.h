#ifndef GAMESCANNER_H
#define GAMESCANNER_H

#include "frontendcommon.h"
#include "archivefile.h"
#include "betterfuturewatcher.h"
#include "metadatadatabase.h"
#include "librarytypes.h"
#include "betterfuturewatcher.h"

#include <QObject>

namespace Library {

    // Phoenix's game scanner takes advantage of MapReduce (and Qt's easy-to-use implementation of it) to efficiently and
    // asyncronously scan large numbers of files. We use the MapFunctor, ReduceFunctor, and FilterFunctor classes
    // to accomplish the map, reduce, and filter tasks. These are defined in their own class files.

    // GameHasher efficiently hashes a set of files as passed to it with the add______() family of slots, automatically
    // passing the results to the next stage, GameMatcher

    class GameHasher : public QObject {
            Q_OBJECT

        public:
            explicit GameHasher( QObject *parent = 0 );

            enum SearchReason {
                GetROMID,
                GetArtwork,
                GetSystemUUID,
                GetMetadata,
                GetHeaders,
            };

            qreal progress() const;
            void setProgress( qreal progress );

            // Searches the OpenVGDB or the LibretroDB, in hopes of finding metadata.
            // The search reasons are listed in the enum. Most of these are used
            // internally. The main search reason to call is GetMetadata.
            // TODO: Move to GameMatcher
            static bool searchDatabase( const SearchReason reason, FileEntry &fileEntry );

        signals:
            void progressChanged( const qreal progress );
            void fileReady( FileEntry entry );

        public slots:
            // Entry points, connect to and invoke these slots to begin the game scanning process
            void addPath( QString path );

        private slots:
            // Handlers that move the process along once done
            void stepOneFinished( BetterFutureWatcher *betterWatcher );
            void stepTwoFinished( BetterFutureWatcher *betterWatcher );
            void stepThreeFinished( BetterFutureWatcher *betterWatcher );
            void stepFourFilterFinished( BetterFutureWatcher *betterWatcher );
            // TODO: Pass results onto GameMatcher
            void stepFourMapReduceFinished( BetterFutureWatcher *betterWatcher );

        private:
            // A list of file paths that have been obtained by scanning the file system.
            FileList mFileList;

            // Our custom way of keeping track of various scanning sessions via these sessions' watchers (BetterFutureWatchers)
            QList<BetterFutureWatcher *> mWatcherList;

            // Maps SHA-1 checksums to a dictionary of metadata (other hashes, filename, system, region) for firmware (BIOS) files

            qreal mTotalProgess;
            int mFilesProcessing;

            // Helpers
            static QString getLastExecutedQuery( const QSqlQuery &query );


    };
}

#endif // GAMESCANNER_H
