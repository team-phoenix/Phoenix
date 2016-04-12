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
            // Entry point, connect to and invoke this slot to begin the game scanning process
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

            void handleProgressValueChanged( int progress );

        private:
            // A list of watchers, each of which monitors a pipeline as it performs operations on its list
            QList<ListWatcher *> mWatcherList;

            // Copies of the main list from step 2 for use by step 3
            // Indexed by the ListWatcher addresses
            QMap<ListWatcher *, FileList> mainLists;

            // Progress monitoring

            struct progressTuple {
                // 0-based
                int step;

                // [0.0, 1.0]
                qreal progress;

                // Total number of files to process (this copy needed when key points to deleted memory)
                int total;
            };

            // Tracks the progress of each pipeline (each active pipeline will be represented by exactly one entry in this map)
            QMap<ListWatcher *, progressTuple> progressMap;

            inline void connectProgress( ListWatcher *watcher );

            // Remove oldWatcher from progressMap and add newWatcher with 0.0 progress and the given step
            void resetProgress( ListWatcher *oldWatcher, ListWatcher *newWatcher, int step );

            // Helpers

            ListWatcher *takeFinished( QList<GameHasher::ListWatcher *> &list );
    };
}
