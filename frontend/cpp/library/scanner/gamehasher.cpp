#include "gamehasher.h"

using namespace Library;

GameHasher::GameHasher( QObject *parent )
    : QObject( parent ),
      mTotalProgess( 0.0 ),
      mFilesProcessing( 0 ) {

}

qreal GameHasher::progress() const {
    return mTotalProgess;
}

void GameHasher::setProgress( int progress ) {
    mTotalProgess = progress;

    if( progress % 5 == 0 || progress == 100 ) {
        emit progressChanged( mTotalProgess );
    }
}

void GameHasher::addPath( QString path ) {
    BetterFutureWatcher *watcher = new BetterFutureWatcher();
    QStringList dirs = QStringList( path );
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, QStringList>( dirs, MapFunctor( MapFunctor::One ), ReduceFunctor( ReduceFunctor::One ) );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepOneFinished );

    qCDebug( phxLibrary ) << "Began scan at" << QDateTime::currentDateTime();

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::addPaths( QStringList paths ) {
    BetterFutureWatcher *watcher = new BetterFutureWatcher();
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, QStringList>( paths, MapFunctor( MapFunctor::One ), ReduceFunctor( ReduceFunctor::One ) );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepOneFinished );

    qCDebug( phxLibrary ) << "Began scan at" << QDateTime::currentDateTime();

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::shutdown() {
    QThread::currentThread()->quit();
}

void GameHasher::stepOneFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    qCDebug( phxLibrary ) << "Step one finished. " << fileList.size();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    // No point in starting for an empty list. Abort!!!
    if( fileList.isEmpty() ) {
        return;
    }

    // Start step two
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, FileList>( fileList, MapFunctor( MapFunctor::Two ), ReduceFunctor( ReduceFunctor::Two ) );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepTwoFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::stepTwoFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    int pivot = betterWatcher->listIndex();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( pivot );
    betterWatcher->deleteLater();

    qCDebug( phxLibrary ) << "Step two finished." << fileList.size();

    // Start step three
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, FileList>( fileList, MapFunctor( MapFunctor::Three ), ReduceFunctor( ReduceFunctor::Three ) );

    // Make a copy of the list for step 3 to use once it's done
    mainLists[ watcher ] = fileList;

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepThreeFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );

    // Adjust stored index for each item in the list that has been moved by this list manipulation
    for( BetterFutureWatcher *b : mWatcherList ) {
        b->adjustIndex( pivot );
    }
}

void GameHasher::stepThreeFinished( BetterFutureWatcher *betterWatcher ) {
    FileList binList = betterWatcher->futureWatcher().result();

    // Convert main FileList to a set (mainSet)
    QSet<FileEntry> mainSet = QSet<FileEntry>::fromList( mainLists[ betterWatcher ] );
    mainLists.remove( betterWatcher );

    // Convert returned fileList to a set (binSet)
    QSet<FileEntry> binSet = QSet<FileEntry>::fromList( binList );

    // Compute mainSet - binSet
    // This will remove all .bin files that are part of .cue files and therefore do not need to be hashed, completing step 3
    mainSet = mainSet - binSet;

    // Convert mainSet back to a list
    FileList fileList = mainSet.toList();

    mFilesProcessing += fileList.size();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    int pivot = betterWatcher->listIndex();
    mWatcherList.removeAt( pivot );
    betterWatcher->deleteLater();

    qCDebug( phxLibrary ) << "Step three finished. " << fileList.size();

    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, FileList>( fileList, MapFunctor( MapFunctor::Four ), ReduceFunctor( ReduceFunctor::Four ) );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepFourFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );

    // Adjust stored index for each item in the list that has been moved by this list manipulation
    for( BetterFutureWatcher *b : mWatcherList ) {
        b->adjustIndex( pivot );
    }
}

void GameHasher::stepFourFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    int pivot = betterWatcher->listIndex();

    // Basic cleanup, do not call 'delete', use 'deleteLater'
    mWatcherList.removeAt( pivot );
    betterWatcher->deleteLater();

    qCDebug( phxLibrary ) << "Step four map reduce finished: " << fileList.size();
    qCDebug( phxLibrary ) << "Scan complete, finished scan at" << QDateTime::currentDateTime();
    qCDebug( phxLibrary ) << "Results:";

    // Build these two lists from the result list then send them off via signals for further processing
    FileList manualModeList;
    FileList knownFilesList;

    for( const FileEntry &entry : fileList ) {
        switch( entry.scannerResult ) {
            case GameScannerResult::MultipleSystemUUIDs:
                qCDebug( phxLibrary ) << "manualModeList" << entry;
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

                qCDebug( phxLibrary ) << "knownFilesList" << entry;
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

    //    mFilesProcessing -= fileList.size();
    //    int progress = qFloor( ( fileList.size() / static_cast<qreal>( mFilesProcessing ) ) * 100 );
    //    setProgress( progress );


    // Adjust stored index for each item in the list that has been moved by this list manipulation
    for( BetterFutureWatcher *b : mWatcherList ) {
        b->adjustIndex( pivot );
    }
}

QString GameHasher::getLastExecutedQuery( const QSqlQuery &query ) {
    QString sql = query.executedQuery();
    int nbBindValues = query.boundValues().size();

    for( int i = 0, j = 0; j < nbBindValues; ) {
        int s = sql.indexOf( QLatin1Char( '\'' ), i );
        i = sql.indexOf( QLatin1Char( '?' ), i );

        if( i < 1 ) {
            break;
        }

        if( s < i && s > 0 ) {
            i = sql.indexOf( QLatin1Char( '\'' ), s + 1 ) + 1;

            if( i < 2 ) {
                break;
            }
        } else {
            const QVariant &var = query.boundValue( j );
            QSqlField field( QLatin1String( "" ), var.type() );

            if( var.isNull() ) {
                field.clear();
            } else {
                field.setValue( var );
            }

            QString formatV = query.driver()->formatValue( field );
            sql.replace( i, 1, formatV );
            i += formatV.length();
            ++j;
        }
    }

    return sql;
}
