#include "gamescancontroller.h"
#include "gamefileinfo.h"
#include "cuefile.h"

#include <QtCore>
#include <QtConcurrent>

using namespace Library;

// Step 3: Parse cue files and pass along game files.
QVariantList mapCueFiles( const QString &file ) {
    QVariantList filePathList;
    QFileInfo info( file );

    if( info.suffix() == QStringLiteral( "cue" ) ) {
        // Explore the cue file.
        qDebug() << "Found cue file: " << file;
        QVariantList binFiles = CueFile::parse( file );
        filePathList.append( binFiles );

    } else {
        filePathList.append( file );
    }

    return filePathList;
}

// Step 3.
void reduceCueFiles( QVariantList &outputList, const QVariantList &fileList ) {
    outputList.append( fileList );
}

// Step 2: Parse archive files and pass along game files.
QVariantList mapZippedFiles( const QString &file ) {

    QVariantList filePathList;

    QFileInfo info( file );

    if( info.suffix() == QStringLiteral( "zip" ) ) {
        // Expand and explore the zip file.
        qDebug() << "Found zip file: " << file;

        ArchiveFile::ParseData parsedData = ArchiveFile::parse( file );
        filePathList.append( parsedData );
    } else {
        filePathList.append( file );
    }

    return filePathList;
}

// Step 2: Parse archive files, cue files, and bin files, game files get passed along.
void reduceZippedFiles( QVariantList &outputList, const QStringList &fileList ) {
    outputList.append( fileList );
}

// Step 1: Enumerate files and folders
// Returns the QString filePath in the given path
QVariantList mapScanFiles( const QString &path ) {

    QVariantList filePathList;
    QFileInfo dir( path );

    qDebug() << path;

    if( dir.isFile() ) {
        filePathList.append( path );
        return filePathList;
    }

    // path is a file system directory past this point.
    QDir directory( path );

    if( !directory.exists() ) {
        //emit error( ErrorCode::PathError );
        return QVariantList();
    }

    QDirIterator dirIter( path, GameFileInfo::gameFilter(), QDir::Files, QDirIterator::NoIteratorFlags );

    while( dirIter.hasNext() ) {
        const QString filePath = dirIter.next();

        if( QFile::exists( filePath ) ) {
            filePathList.append( filePath );
        }
    }

    return filePathList;
}

// Step 1: Enumerate files and folders
void reduceScanFiles( QVariantList &pathList, const QStringList &fileList ) {
    if( !fileList.isEmpty() ) {
        pathList.append( fileList );
    }
}


GameScanController::GameScanController( QObject *parent )
    : QObject( parent ),
      mProgress( 0 ) {
    connect( this, &GameScanController::error, this, &GameScanController::handleError );
    connect( this, &GameScanController::threadSafeStartScan, this, &GameScanController::handleScanStarted );
    //connect( &mFutureManager, FutureManager::finished, this, &GameScanController::handleFutureFinished );
}

void GameScanController::appendScanPath( const QString scanPath ) {
    mScanPathList.append( scanPath );
    emit scanPathChanged();
}

void GameScanController::setProgress( int progress ) {
    mProgress = progress;
    emit progressChanged();
}

int GameScanController::progress() {
    return mProgress;
}

QStringList GameScanController::scanPath() const {
    return mScanPathList;
}

void GameScanController::startScan() {
    emit threadSafeStartScan();
}

void GameScanController::handleError( const GameScanController::ErrorCode error ) {
    switch( error ) {
        case ErrorCode::PathError:
            qCDebug( phxLibrary ) << "Import path does not exist";
            break;

        case ErrorCode::TypeError:
            qCDebug( phxLibrary ) << "Import path is a file.";
            break;

        default:
            Q_UNREACHABLE();
            break;
    }
}

// Step 1: Enumerate files and folders
void GameScanController::handleScanStarted() {

    qDebug() << "Started " << Q_FUNC_INFO;

    QFutureWatcher<QVariantList> *watcher = new QFutureWatcher<QVariantList>;

    connect( watcher, SIGNAL( finished() ), this, SLOT( handleEnumerateFilesFinished() ) );
    QFuture<QVariantList> future = QtConcurrent::mappedReduced( mScanPathList, mapScanFiles, reduceScanFiles );


    mWatcherList.append( watcher );

    watcher->setFuture( future );


}

// Finish up with Step 1, prepare for moving onto Step 2.
void GameScanController::handleEnumerateFilesFinished() {
    QVariantList result = watcherResult();

    // Move onto Step 2.
    parseArchiveFiles( result );
}

// Finish up with Step 2, prepare for moving onto Step 3.s
void GameScanController::handleParseFilesFinished() {
    QList<ArchiveFile::ParseData> result = qvariant_cast<QList<ArchiveFile::ParseData>>( watcherResult() );

    qDebug() << Q_FUNC_INFO << result;


    // Move onto Step 3.
    parseCueFiles( result );
}

void GameScanController::handleParseCueFilesFinished() {
    QStringList result = watcherResult().toStringList();
    qDebug() << Q_FUNC_INFO << result;
}

// Step 2: Parse archive files, cue files, and bin files, game files get passed along.
void GameScanController::parseArchiveFiles( QVariantList result ) {

    //qDebug() << "Finished scan" << Q_FUNC_INFO << result;

    QFutureWatcher<QVariantList> *watcher = new QFutureWatcher<QVariantList>;

    connect( watcher, SIGNAL( finished() ), this, SLOT( handleParseFilesFinished() ) );

    QFuture<QVariant> future = QtConcurrent::mappedReduced( result, mapZippedFiles, reduceZippedFiles );

    mWatcherList.append( watcher );

    watcher->setFuture( future );


}

// Step 3:
void GameScanController::parseCueFiles( QStringList result ) {

    QFutureWatcher<QVariant> *watcher = new QFutureWatcher<QVariant>;

    connect( watcher, SIGNAL( finished() ), this, SLOT( handleParseCueFilesFinished() ) );

    QFuture<QVariant> future = QtConcurrent::mappedReduced( result, mapCueFiles, reduceCueFiles );

    mWatcherList.append( watcher );
    watcher->setFuture( future );

}

QVariantList GameScanController::watcherResult() {
    QVariantList result;

    for( int i = 0; i < mWatcherList.size(); ++i ) {
        WatcherPtr watcher = mWatcherList.at( i );

        if( watcher->isFinished() ) {
            result.append( watcher->result() );

            // Clean up so no memory leaks occur.
            watcher->deleteLater();
            mWatcherList.removeAt( i );
            break;
        }
    }

    return result;
}

