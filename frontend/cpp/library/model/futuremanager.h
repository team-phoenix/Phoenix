#ifndef FUTUREMANAGER_H
#define FUTUREMANAGER_H

#include <QObject>

#include <QFutureWatcher>

/*
using WatcherPtr = QFutureWatcher<QStringList> *;

class FutureManager : public QObject
{
    Q_OBJECT
public:
    explicit FutureManager( QObject *parent = nullptr );

    enum ScanStep {
        EnumerateFolders = 0,
        ParseFiles,
    };

    // The QFuture provided to this function cannot go out of scope.
    // FutureManager has to have ownership of these futures.
    // FutureManager will delete these pointers.
    void setFutureWatcher( QFutureWatcher<QStringList> *future );

    const QList<WatcherPtr> &watcherList() const
    {
        return mWatcherList;
    }

public slots:
    void emitProgress( const int progress );
    void cancel();

    void pause();

    void resume();

private slots:
    void handleWatcherFinished();

signals:
    void progressChanged( int progress );
    void finished( QStringList result );

private:
    QList<WatcherPtr> mWatcherList;
    int mProgress;
};
*/
#endif // FUTUREMANAGER_H
