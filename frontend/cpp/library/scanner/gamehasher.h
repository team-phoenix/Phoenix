#pragma once

#include "frontendcommon.h"

#include "archivefile.h"
#include "betterfuturewatcher.h"
#include "cryptohash.h"
#include "cuefile.h"
#include "librarytypes.h"
#include "libretrodatabase.h"
#include "metadatadatabase.h"
#include "phxpaths.h"

#include "mapfunctor.h"
#include "filterfunctor.h"
#include "reducefunctor.h"

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

            qreal progress() const;
            void setProgress( qreal progress );

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
            // TODO: Pass results onto GameMatcher
            void stepFourFinished( BetterFutureWatcher *betterWatcher );

        private:
            // Our custom way of keeping track of various scanning sessions via these sessions' watchers (BetterFutureWatchers)
            QList<BetterFutureWatcher *> mWatcherList;

            // Copies of the main list from step 2 for use by step 3
            // Indexed by betterFutureWatcher addresses
            QMap<BetterFutureWatcher *, FileList> mainLists;

            qreal mTotalProgess;
            int mFilesProcessing;

            // Helpers
            static QString getLastExecutedQuery( const QSqlQuery &query );
    };
}
