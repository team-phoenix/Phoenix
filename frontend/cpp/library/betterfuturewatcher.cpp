#include "betterfuturewatcher.h"
#include <QDebug>

using namespace Library;

BetterFutureWatcher::BetterFutureWatcher( QObject *parent )
    : QObject( parent ),
      mListIndex( -1 ) {
    connect( &mWatcher, SIGNAL( finished() ), this, SLOT( slotInterceptFinished() ) );
    connect( &mWatcher, SIGNAL( progressValueChanged( int ) ), this, SLOT( slotForwardProgress( int ) ) );
}

void BetterFutureWatcher::setFuture( const QFuture<FileList> &future, int index ) {
    mWatcher.setFuture( future );
    mListIndex = index;
}

const QFutureWatcher<FileList> &BetterFutureWatcher::futureWatcher() {
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

void BetterFutureWatcher::slotForwardProgress( int progress ) {
    qreal val = progress / static_cast<qreal>( mWatcher.progressMaximum() );
    emit progressChanged( static_cast<int>( val * 100 ) );

    qDebug() << "RESULT SIZE: " << mWatcher.future().resultCount();

}
