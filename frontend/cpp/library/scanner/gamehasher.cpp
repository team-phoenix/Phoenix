#include "gamehasher.h"

#include "archivefile.h"
#include "cryptohash.h"
#include "cuefile.h"
#include "libretrodatabase.h"
#include "metadatadatabase.h"
#include "phxpaths.h"

#include "mapfunctor.h"
#include "filterfunctor.h"
#include "reducefunctor.h"

using namespace Library;

GameHasher::GameHasher( QObject *parent )
    : QObject( parent ),
      mFilesProcessings( 0 )
{
}

void GameHasher::addPath( QString path ) {
    emit running( true );
    QStringList dirs = QStringList( path );

    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, QStringList>( dirs
                                                                                   , MapFunctor( MapFunctor::Step::One )
                                                                                   , ReduceFunctor( ReduceFunctor::Step::One ) );
    ListWatcher *watcher = new ListWatcher;
    connect( watcher, &ListWatcher::finished, this, &GameHasher::stepOneFinished );

    qCDebug( phxLibrary ) << "Began scan at" << QDateTime::currentDateTime();

    watcher->setFuture( future );
    mWatcherList.append( watcher );
}

void GameHasher::addPaths( QStringList paths ) {
    emit running( true );

    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, QStringList>( paths
                                                                                   , MapFunctor( MapFunctor::Step::One )
                                                                                   , ReduceFunctor( ReduceFunctor::Step::One ) );
    ListWatcher *watcher = new ListWatcher;
    connect( watcher, &ListWatcher::finished, this, &GameHasher::stepOneFinished );

    qCDebug( phxLibrary ) << "Began scan at" << QDateTime::currentDateTime();

    watcher->setFuture( future );
    mWatcherList.append( watcher );
}

void GameHasher::pause() {
    for( ListWatcher *watcher : mWatcherList ) {
        watcher->pause();
    }
}

void GameHasher::cancel() {
    for( int i=0; i < mWatcherList.size(); ++i ) {
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

    newWatcher->setFuture( future );
    mWatcherList.append( newWatcher );

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

    newWatcher->setFuture( future );
    mWatcherList.append( newWatcher );

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

}

void GameHasher::stepFourFinished() {
    ScopedWatcher watcher( takeFinished( mWatcherList ) );
    if ( watcher.isNull() ) {
        return;
    }

    FileList fileList = watcher->result();
    mFilesProcessings -= fileList.size();

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

    if ( mWatcherList.isEmpty() ) {
        Q_ASSERT( mFilesProcessings == 0 );
        emit running( false );
        emit progressChanged( 0 );
    }

}

void GameHasher::handleProgressRangeChanged( int min, int max ) {
    Q_UNUSED( min );
    mFilesProcessings += max;
}

void GameHasher::handleProgressValueChanged( int progress ) {
    emit progressChanged( qFloor( ( progress / static_cast<qreal>( mFilesProcessings ) ) * 500 ) );
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

GameHasher::ListWatcher *GameHasher::takeFinished(QList<ListWatcher *> &list) {
    ListWatcher *result = nullptr;
    for( int i=0; i < list.size(); ++i ) {
        ListWatcher *watcher = list[ i ];
        if ( watcher->isFinished() ) {
            result = watcher;
            list.removeAt( i );
            break;
        }
    }

    return result;
}
