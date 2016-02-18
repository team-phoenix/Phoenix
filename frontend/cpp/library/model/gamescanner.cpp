#include "gamescanner.h"
#include "cuefile.h"

#include <QtConcurrent>


GameScanner::GameScanner( QObject *parent )
    : QObject( parent ) {

}

FileList GameScanner::stepOneMap( const QString &path ) {

    FileList filePathList;
    QFileInfo dir( path );

    qDebug() << "path: " << path;

    if( dir.isFile() ) {
        FileEntry entry;
        entry.filePath = path;
        filePathList.append( entry );
        return filePathList;
    }

    // path is a file system directory past this point.
    QDir directory( path );

    if( !directory.exists() ) {
        return FileList();
    }

    QDirIterator dirIter( path, QDir::Files, QDirIterator::NoIteratorFlags );

    while( dirIter.hasNext() ) {
        const QString filePath = dirIter.next();

        if( QFile::exists( filePath ) ) {
            FileEntry entry;
            entry.filePath = filePath;
            filePathList.append( entry );
        }
    }

    return filePathList;
}

void GameScanner::stepOneReduce( FileList &mergedList, const FileList &givenList ) {
    if( !givenList.isEmpty() ) {
        mergedList.append( givenList );
    }
}

FileList GameScanner::stepTwoMap( const FileEntry &filePath ) {
    FileList filePathList;

    QFileInfo info( filePath.filePath );

    if( info.suffix() == QStringLiteral( "zip" ) ) {
        // Expand and explore the zip file.

        ArchiveFile::ParseData parsedData = ArchiveFile::parse( filePath.filePath );
        for ( QString &file : parsedData.enumeratedFiles ) {
            FileEntry entry;
            entry.filePath = file;
            filePathList.append( entry );

        }

    } else {
        filePathList.append( filePath );
    }


    return filePathList;
}

void GameScanner::stepTwoReduce( FileList &mergedList, const FileList &givenList ) {
    mergedList.append( givenList );
}

FileList GameScanner::stepThreeMap( const FileEntry &entry )  {

    FileList filePathList;

    QFileInfo info( entry.filePath );
    if( info.suffix() == QStringLiteral( "cue" ) ) {
        // Explore the cue file.
        qDebug() << "Found cue file: " << entry.filePath;
        QStringList binFiles = CueFile::parse( entry.filePath );
        for ( QString binFile : binFiles ) {
            FileEntry newEntry;
            newEntry.filePath = binFile;
            filePathList.append( newEntry );
        }
    } else {
        filePathList.append( entry );
    }

    return filePathList;
}

void GameScanner::stepThreeReduce( FileList &mergedList, const FileList &givenList) {
    mergedList.append( givenList );
}

bool GameScanner::stepFourFilter( const FileEntry &fileEntry ) {

    QFileInfo info( fileEntry.filePath );


    if ( info.suffix() == QStringLiteral( "bin" ) ) {
        // Check for bios, cache if bios is found

        return false;
    }

    return true;

}

void GameScanner::stepFourFilterReduce( FileList &mergedList, const FileEntry &givenList ) {
    mergedList.append( givenList );
}

void GameScanner::slotEnumeratePath( QString path ) {

    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QStringList dirs = QStringList(path);
    QFuture<FileList> future = QtConcurrent::mappedReduced( dirs, GameScanner::stepOneMap, GameScanner::stepOneReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameScanner::stepOneFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameScanner::stepOneFinished( BetterFutureWatcher *betterWatcher ) {

    FileList fileList = betterWatcher->futureWatcher().result();

    qDebug() << "Step 1 finished. " << fileList.size();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    // No point in starting for an empty list. Abort!!!
    if ( fileList.isEmpty() ) {
        return;
    }

    // Start for step two
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced( fileList, GameScanner::stepTwoMap, GameScanner::stepTwoReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameScanner::stepTwoFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );

}

void GameScanner::stepTwoFinished( BetterFutureWatcher *betterWatcher ) {

    FileList fileList = betterWatcher->futureWatcher().result();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    qDebug() << "Step two finished." << fileList.size();

    // Start for step three
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced( fileList, GameScanner::stepThreeMap, GameScanner::stepThreeReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameScanner::stepThreeFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );


}

void GameScanner::stepThreeFinished( BetterFutureWatcher *betterWatcher ) {

    FileList fileList = betterWatcher->futureWatcher().result();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    qDebug() << "Step three finished. " << fileList.size();

    // Start for step three
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::filteredReduced( fileList, GameScanner::stepFourFilter, GameScanner::stepFourFilterReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameScanner::stepFourFilterFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );


}

void GameScanner::stepFourFilterFinished( BetterFutureWatcher *betterWatcher )
{
    FileList fileList = betterWatcher->futureWatcher().result();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    qDebug() << "Step four filter finished. " << fileList.size();
}

void GameScanner::stepFourMapReduceFinished( BetterFutureWatcher *betterWatcher )
{
    FileList fileList = betterWatcher->futureWatcher().result();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    qDebug() << "Step four map reduce finished. " << fileList.size();
}

