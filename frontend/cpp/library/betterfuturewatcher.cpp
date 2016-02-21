#include "betterfuturewatcher.h"

using namespace Library;

BetterFutureWatcher::BetterFutureWatcher( QObject *parent )
    : QObject( parent ),
      mListIndex( -1 ) {
    connect( &mWatcher, SIGNAL( finished() ), this, SLOT( slotInterceptFinished() ) );
}

void BetterFutureWatcher::setFuture( const QFuture<FileList> &future, int index ) {
    mWatcher.setFuture( future );
    mListIndex = index;
}

QFutureWatcher<FileList> &BetterFutureWatcher::futureWatcher() {
    return mWatcher;
}

int BetterFutureWatcher::listIndex() const {
    return mListIndex;
}

void BetterFutureWatcher::adjustIndex( int pivot ) {
    if( mListIndex > pivot ) {
        mListIndex--;
    }
}

void Library::BetterFutureWatcher::slotInterceptFinished() {
    emit finished( this );
}
