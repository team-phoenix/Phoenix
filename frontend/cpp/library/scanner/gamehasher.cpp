#include "gamehasher.h"

#include "archivefile.h"
#include "cryptohash.h"
#include "cuefile.h"
#include "phxpaths.h"

#include "mapfunctor.h"
#include "reducefunctor.h"
#include "logging.h"

using namespace Library;

using ScopedWatcher = QScopedPointer<GameHasher::ListWatcher, QScopedPointerDeleteLater>;

GameHasher::GameHasher( QObject *parent )
    : QObject( parent ) {
}

void GameHasher::addPaths( QStringList paths ) {
    emit running( true );

    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, QStringList>( paths
                               , MapFunctor( MapFunctor::Step::One )
                               , ReduceFunctor( ReduceFunctor::Step::One ) );
    ListWatcher *watcher = new ListWatcher;
    connect( watcher, &ListWatcher::finished, this, &GameHasher::stepOneFinished );
    connectProgress( watcher );

    qCDebug( phxLibrary ) << "Began scan at" << QDateTime::currentDateTime();

    watcher->setFuture( future );

    // Reset progress monitoring if there's nothing already being scanned
    if( mWatcherList.isEmpty() ) {
        emit progressChanged( 0 );
    }

    mWatcherList.append( watcher );

    // Begin progress monitoring this pipeline
    resetProgress( nullptr, watcher, 0 );
}

void GameHasher::pause() {
    for( ListWatcher *watcher : mWatcherList ) {
        watcher->pause();
    }
}

void GameHasher::cancel() {
    for( int i = 0; i < mWatcherList.size(); ++i ) {
        ScopedWatcher watcher( mWatcherList.takeAt( i ) );
        watcher->cancel();
        watcher->waitForFinished();
    }

    emit running( false );
    emit progressChanged( 0 );
}

void GameHasher::resume() {
    for( ListWatcher *watcher : mWatcherList ) {
        watcher->resume();
    }
}

void GameHasher::shutdown() {
    if( mWatcherList.size() > 0 ) {
        qCDebug( phxLibrary ) << "Canceling active scan(s)";
        cancel();
    } else {
        qCDebug( phxLibrary ) << "No scan active, continuing";
    }

    QThread::currentThread()->quit();

    qCDebug( phxLibrary ) << "Fully unloaded";
}

void GameHasher::stepOneFinished() {
    ScopedWatcher watcher( takeFinished( mWatcherList ) );

    if( watcher.isNull() ) {
        return;
    }

    FileList fileList = watcher->result();

    qCDebug( phxLibrary ) << "Step one finished. " << fileList.size();

    // No point in starting for an empty list. Abort!!!
    if( fileList.isEmpty() ) {
        return;
    }

    // Start step two
    ListWatcher *newWatcher = new ListWatcher;
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, FileList>( fileList
                               , MapFunctor( MapFunctor::Step::Two )
                               , ReduceFunctor( ReduceFunctor::Step::Two ) );

    connect( newWatcher, &ListWatcher::finished, this, &GameHasher::stepTwoFinished );
    connectProgress( newWatcher );

    newWatcher->setFuture( future );
    mWatcherList.append( newWatcher );
    resetProgress( watcher.data(), newWatcher, 1 );
}

void GameHasher::stepTwoFinished() {
    ScopedWatcher watcher( takeFinished( mWatcherList ) );

    if( watcher.isNull() ) {
        return;
    }

    FileList fileList = watcher->result();

    qCDebug( phxLibrary ) << "Step two finished." << fileList.size();

    // Start step three
    ListWatcher *newWatcher = new ListWatcher;
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, FileList>( fileList
                               , MapFunctor( MapFunctor::Step::Three )
                               , ReduceFunctor( ReduceFunctor::Step::Three ) );

    // Make a copy of the list for step 3 to use once it's done
    mainLists[ newWatcher ] = fileList;

    connect( newWatcher, &ListWatcher::finished, this, &GameHasher::stepThreeFinished );
    connectProgress( newWatcher );

    newWatcher->setFuture( future );
    mWatcherList.append( newWatcher );
    resetProgress( watcher.data(), newWatcher, 2 );
}

void GameHasher::stepThreeFinished() {
    ScopedWatcher watcher( takeFinished( mWatcherList ) );

    if( watcher.isNull() ) {
        return;
    }

    FileList binList = watcher->result();

    // Convert main FileList to a set (mainSet)
    QSet<FileEntry> mainSet = QSet<FileEntry>::fromList( mainLists[ watcher.data() ] );
    mainLists.remove( watcher.data() );

    // Convert returned fileList to a set (binSet)
    QSet<FileEntry> binSet = QSet<FileEntry>::fromList( binList );

    // Compute mainSet - binSet
    // This will remove all .bin files that are part of .cue files and therefore do not need to be hashed, completing step 3
    mainSet = mainSet - binSet;

    // Convert mainSet back to a list
    FileList fileList = mainSet.toList();

    qCDebug( phxLibrary ) << "Step three finished. " << fileList.size();

    ListWatcher *newWatcher = new ListWatcher;
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, FileList>( fileList
                               , MapFunctor( MapFunctor::Step::Four )
                               , ReduceFunctor( ReduceFunctor::Step::Four ) );

    connect( newWatcher, &ListWatcher::finished, this, &GameHasher::stepFourFinished );
    connectProgress( newWatcher );

    newWatcher->setFuture( future );
    mWatcherList.append( newWatcher );
    resetProgress( watcher.data(), newWatcher, 3 );
}

void GameHasher::stepFourFinished() {
    ScopedWatcher watcher( takeFinished( mWatcherList ) );

    if( watcher.isNull() ) {
        return;
    }

    FileList fileList = watcher->result();

    qCDebug( phxLibrary ) << "Step four map reduce finished: " << fileList.size();
    qCDebug( phxLibrary ) << "Scan complete, finished scan at" << QDateTime::currentDateTime();
    qCDebug( phxLibrary ) << "Results:";

    // Build these two lists from the result list then send them off via signals for further processing
    FileList manualModeList;
    FileList knownFilesList;

    for( const FileEntry &entry : fileList ) {
        switch( entry.scannerResult ) {
            case GameScannerResult::MultipleSystemUUIDs:
                //qCDebug( phxLibrary ) << "manualModeList" << entry;
                manualModeList.append( entry );
                break;

            // Ignore games that do not match any system by our scanner (including by extension)
            // Also ignore games that match by game UUID but have their system UUID list empty (disabled system)
            case GameScannerResult::SystemUUIDUnknown:
                break;

            default:
                if( entry.systemUUIDs.isEmpty() ) {
                    break;
                }

                knownFilesList.append( entry );
                break;
        }
    }

    qCDebug( phxLibrary ) << "End of list";
    qCDebug( phxLibrary ) << "Games with one system UUID:" << knownFilesList.size();
    qCDebug( phxLibrary ) << "Games with multiple system UUIDs:" << manualModeList.size();

    if( !manualModeList.isEmpty() ) {
        emit filesNeedAssignment( manualModeList );
    }

    if( !knownFilesList.isEmpty() ) {
        emit scanCompleted( knownFilesList );
    }

    // Keep on the map so its 100% progress is retained
    progressMap[ watcher.data() ].progress = 1.0;

    if( mWatcherList.isEmpty() ) {
        progressMap.clear();
        emit running( false );
    }

}

void GameHasher::handleProgressValueChanged( int progress ) {
    // Grab the ListWatcher that invoked this slot
    ListWatcher *watcher = dynamic_cast<ListWatcher *>( sender() );

    // Update this ListWatcher's progress in our map
    progressMap[ watcher ].progress = ( qreal )progress / watcher->progressMaximum();

    int totalFiles = 0;
    int completedFiles = 0;

    // Sum up all the progresses
    // Do not dereference key, it may already be deleted!
    for( ListWatcher *key : progressMap.keys() ) {
        totalFiles += progressMap[ key ].total;
        // Bias the completion amount by what step this pipeline is currently in
        completedFiles += progressMap[ key ].total
                          * ( ( ( qreal )( progressMap[ key ].progress ) / 4.0 )
                              + ( 0.25 * ( qreal )( progressMap[ key ].step ) ) );

        // Only count step 4
        // completedFiles += progressMap[ key ].total
        //                   * ( ( progressMap[ key ].step ) == 3
        //                       ? ( qreal )( progressMap[ key ].progress )
        //                       : 0.0 );
    }

    emit progressChanged( qFloor( ( ( qreal )completedFiles / ( qreal )totalFiles ) * 500.0 ) );
}

void Library::GameHasher::connectProgress( ListWatcher *watcher ) {
    connect( watcher, &ListWatcher::progressValueChanged, this, &GameHasher::handleProgressValueChanged );
}

void GameHasher::resetProgress( ListWatcher *oldWatcher, ListWatcher *newWatcher, int step ) {
    // Remove old watcher from map
    if( oldWatcher ) {
        progressMap.remove( oldWatcher );
    }

    // Add new watcher to list
    if( newWatcher ) {
        progressMap[ newWatcher ].step = step;
        progressMap[ newWatcher ].progress = 0.0;
        progressMap[ newWatcher ].total = newWatcher->progressMaximum();
    }
}

GameHasher::ListWatcher *GameHasher::takeFinished( QList<ListWatcher *> &list ) {
    // Two ListWatchers will never be finished at the same time
    // because of the event loop. This is why we can just break out of the loop.

    ListWatcher *result = nullptr;

    for( int i = 0; i < list.size(); ++i ) {
        ListWatcher *watcher = list[ i ];

        if( watcher->isFinished() ) {
            result = watcher;
            list.removeAt( i );
            break;
        }
    }

    return result;
}
