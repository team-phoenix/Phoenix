#include "futuremanager.h"

/*
FutureManager::FutureManager( QObject *parent )
    : QObject( parent ),
      mProgress( 0 )
{
    qRegisterMetaType<WatcherPtr>( "WatcherPtr" );
}

// The future needs to be properly deleted.
void FutureManager::setFutureWatcher( QFutureWatcher<QStringList> *watcher )
{
    connect( watcher, SIGNAL(progressValueChanged(int)), this, SLOT(emitProgress(int)) );
    connect( watcher, SIGNAL(finished()), this, SLOT( handleWatcherFinished() ) );
    mWatcherList.append( watcher );
}

void FutureManager::emitProgress( int progress ) {
    Q_UNUSED( progress )

    int sum = 0;
    for ( WatcherPtr watcher : mWatcherList ) {
        sum += watcher->progressValue();
    }

    qDebug() << "progress: " << progress;
    emit progressChanged( sum / mWatcherList.size() );
}

void FutureManager::cancel() {
    for ( WatcherPtr watcher : mWatcherList ) {
        watcher->cancel();
    }
}

void FutureManager::pause() {
    for ( WatcherPtr watcher : mWatcherList ) {
        watcher->pause();
    }
}

void FutureManager::resume() {
    for ( WatcherPtr watcher : mWatcherList ) {
        watcher->resume();
    }
}

void FutureManager::handleWatcherFinished()
{
    qDebug() << "Handle finish";

    for ( int i=0; i < mWatcherList.size(); ++i ) {

        WatcherPtr watcher = mWatcherList.at( i );

        if ( watcher->isFinished() ) {

            QStringList result = watcher->result();

            emit finished( result, );

            // Clean up so no memory leaks occur.
            watcher->deleteLater();
            mWatcherList.removeAt( i );
            break;
        }

    }


}
*/

