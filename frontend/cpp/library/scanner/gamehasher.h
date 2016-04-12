#pragma once

#include "librarytypes.h"

#include <QFutureWatcher>


namespace Library {

    // Phoenix's game scanner takes advantage of MapReduce (and Qt's easy-to-use implementation of it) to efficiently and
    // asyncronously scan large numbers of files. We use the MapFunctor, ReduceFunctor, and FilterFunctor classes
    // to accomplish the map, reduce, and filter tasks. These are defined in their own class files.

    // GameHasher efficiently hashes a set of files as passed to it with the add______() family of slots, automatically
    // passing the results to the next stage, GameMatcher


    class GameHasher : public QObject {
            Q_OBJECT
        public:
            using ListWatcher = QFutureWatcher<FileList>;

            explicit GameHasher( QObject *parent = nullptr );

        signals:
            void running( bool );
            void progressChanged( int progress );
            void fileReady( FileEntry entry );
            void scanCompleted( FileList results );
            void filesNeedAssignment( FileList results );

        public slots:
            // Entry points, connect to and invoke these slots to begin the game scanning process
            void addPath( QString path );
            void addPaths( QStringList paths );

            void pause();
            void cancel();
            void resume();

            // Clean up and exit
            void shutdown();

        private slots:
            // Handlers that move the process along once done
            void stepOneFinished();
            void stepTwoFinished();
            void stepThreeFinished();
            // TODO: Pass results onto GameMatcher
            void stepFourFinished();

            void handleProgressRangeChanged( int min, int max );
            void handleProgressValueChanged( int progress );

        private:
            // Our custom way of keeping track of various scanning sessions via these sessions' watchers (ListWatcher)
            QList<ListWatcher *> mWatcherList;

            // Copies of the main list from step 2 for use by step 3
            // Indexed by the ListWatcher addresses
            QMap<ListWatcher *, FileList> mainLists;

            int mFilesProcessings;

            // Helpers
            static QString getLastExecutedQuery( const QSqlQuery &query );
            ListWatcher *takeFinished( QList<GameHasher::ListWatcher *> &list );

            inline void connectProgress( ListWatcher *watcher );

    };

    void GameHasher::connectProgress( ListWatcher *watcher ) {
        connect( watcher, &ListWatcher::progressValueChanged, this, &GameHasher::handleProgressValueChanged );
        connect( watcher, &ListWatcher::progressRangeChanged, this, &GameHasher::handleProgressRangeChanged );
    }

}
