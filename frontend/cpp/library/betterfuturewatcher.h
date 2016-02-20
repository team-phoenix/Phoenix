#ifndef BETTERFUTUREWATCHER_H
#define BETTERFUTUREWATCHER_H

#include <QObject>
#include <QFutureWatcher>

#include "librarytypes.h"

namespace Library {

    class BetterFutureWatcher : public QObject {
        Q_OBJECT
    public:
        BetterFutureWatcher( QObject *parent = 0 );

        void setFuture( const QFuture<FileList> &future, int index );

        QFutureWatcher<FileList> &futureWatcher();

        int listIndex() const;

    signals:
        void finished( BetterFutureWatcher *watcher  );

    public slots:
        void slotInterceptFinished();


    private:
        QFutureWatcher<FileList> mWatcher;
        int mListIndex;

    };

}
#endif // BETTERFUTUREWATCHER_H
