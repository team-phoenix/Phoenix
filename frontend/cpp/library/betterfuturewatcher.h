#ifndef BETTERFUTUREWATCHER_H
#define BETTERFUTUREWATCHER_H

#include <QObject>
#include <QFutureWatcher>

#include "librarytypes.h"

namespace Library {

    // BetterFutureWatcher is just a wrapper around a QFutureWatcher that is used by the GameHasher class.
    // In GameHasher, we originally stored pointers of QFutureWatcher's inside of a QList
    // in order to keep track of how many were being used, and to retrieve the result once
    // the process had finished. QFutureWatcher's finished() signal doesn't emit a pointer to itself, and so
    // we had to iterate through every QFutureWatcher to check which one was finished. This is an optimization
    // that will itercept and reemit a finished() signal that has a pointer to the FutureWatcher. This
    // class also keeps track of what index it is in the list, so that clean up and retrieval is a O(1) operation.
    // instead of O(n).

    class BetterFutureWatcher : public QObject {
        Q_OBJECT
    public:
        BetterFutureWatcher( QObject *parent = 0 );

        void setFuture( const QFuture<FileList> &future, int index );

        QFutureWatcher<FileList> &futureWatcher();

        int listIndex() const;
        void adjustIndex( int pivot );

    signals:
        void finished( BetterFutureWatcher *watcher  );
        void progress( qreal progress );

    public slots:
        void slotInterceptFinished();


    private:
        QFutureWatcher<FileList> mWatcher;
        int mListIndex;

    };

}
#endif // BETTERFUTUREWATCHER_H
