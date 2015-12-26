#include "gamescannerold.h"

using namespace Library;

GameScannerOld::GameScannerOld( QObject *parent )
    : QObject( parent ),
      mInsertCancelled( false ),
      mInsertPaused( false ),
      mRunning( false ),
      qmlResumeQuitScan( false ) {

    connect( this, &GameScannerOld::started, this, [ this ] {
        setIsRunning( true );
    } );

    connect( this, &GameScannerOld::finished, this, [ this ] {
        setIsRunning( false );
        mFileInfoQueue.clear();
        setInsertCancelled( false );
    } );

}

GameScannerOld::~GameScannerOld() {

    if( !resumeQuitScan() ) {
        setResumeDirectory( "" );
        setResumeInsertID( "" );
    }

    QSettings settings;
    settings.beginGroup( QStringLiteral( "Library" ) );
    settings.setValue( QStringLiteral( "ResumeInsertID" ), getResumeInsertID() );
    settings.setValue( QStringLiteral( "ResumeDirectory" ), getResumeDirectory() );

}

void GameScannerOld::eventLoopStarted() {

    if( resumeQuitScan() ) {
        QSettings settings;

        settings.beginGroup( QStringLiteral( "Library" ) );
        auto value = settings.value( QStringLiteral( "ResumeInsertID" ) );
        auto lastUsedPath = settings.value( QStringLiteral( "ResumeDirectory" ) );

        if( value.isValid() && lastUsedPath.isValid() ) {
            setResumeInsertID( value.toString() );
            setResumeDirectory( lastUsedPath.toString() );

            if( !getResumeInsertID().isEmpty() ) {
                qCDebug( phxLibrary ) << "Resuming interrupted scan of" << getResumeDirectory();

                // Scan the thing we were supposed to scan before, scan as a file if it's a file
                if( scanFolder( getResumeDirectory(), true ) == 1 ) {
                    hashAndEnqueueFile( getResumeDirectory() );
                }
            }
        }
    }
}

void GameScannerOld::handleDraggedUrls( QList<QUrl> urls ) {
    mDraggedUrls = urls;
}

void GameScannerOld::handleDroppedUrls() {

    qCDebug( phxLibrary ) << "Message received:" << mDraggedUrls.size() << "files and/or folders have been passed via drag & drop. Calculating their hashes...";

    for( auto &url : mDraggedUrls ) {

        auto localUrl = url.toLocalFile();

        // Process a file if given a file
        if( scanFolder( localUrl, false ) == 1 ) {
            hashAndEnqueueFile( localUrl );
        }

    }

    emit started();
    prepareGameData( mFileInfoQueue );
    emit finished();
}

void GameScannerOld::handleContainsDrag( const bool contains ) {
    if( !contains ) {
        mDraggedUrls.clear();
    }
}

bool GameScannerOld::resumeQuitScan() {
    QMutexLocker locker( &mMutex );
    return qmlResumeQuitScan;
}

void GameScannerOld::setResumeQuitScan( const bool resume ) {
    QMutexLocker locker( &mMutex );
    qmlResumeQuitScan = resume;
}

QString GameScannerOld::getResumeDirectory() {
    QMutexLocker locker( &mMutex );
    return mResumeDirectory;;
}

void GameScannerOld::setResumeDirectory( const QString directory ) {
    QMutexLocker locker( &mMutex );
    mResumeDirectory = directory;
}

QString GameScannerOld::getResumeInsertID() {
    QMutexLocker locker( &mMutex );
    return mResumeInsertID;
}

void GameScannerOld::setResumeInsertID( const QString id ) {
    QMutexLocker locker( &mMutex );
    mResumeInsertID = id;
}

bool GameScannerOld::isRunning() {
    QMutexLocker locker( &mMutex );
    return mRunning;
}

void GameScannerOld::setIsRunning( const bool running ) {
    QMutexLocker locker( &mMutex );
    mRunning = running;
}

bool GameScannerOld::insertCancelled() {
    QMutexLocker locker( &mMutex );
    return mInsertCancelled;
}

bool GameScannerOld::insertPaused() {
    QMutexLocker locker( &mMutex );
    return mInsertPaused;
}

void GameScannerOld::setInsertCancelled( const bool cancelled ) {
    QMutexLocker locker( &mMutex );

    if( cancelled ) {
        //mResumeDirectory.clear();
        //mResumeInsertID.clear();
    }

    mInsertCancelled = cancelled;
}

void GameScannerOld::setInsertPaused( const bool paused ) {
    QMutexLocker locker( &mMutex );
    mInsertPaused = paused;
}

int GameScannerOld::scanFolder( const QString path, bool autoStart = true ) {

    // Check that the directory exists before continuing
    QDir directory( path );

    QFileInfo dir( path );

    if( dir.isFile() ) {
        return 1;
    }

    if( !directory.exists() ) {
        qCWarning( phxLibrary ) << path << " does not exist!";
        return 2;
    }

    QDirIterator dirIter( path, GameFileInfo::gameFilter(), QDir::Files, QDirIterator::NoIteratorFlags );

    if( resumeQuitScan() && !getResumeInsertID().isEmpty() ) {

        bool insert = false;

        while( dirIter.hasNext() ) {
            auto localFile = dirIter.next();

            if( localFile == getResumeInsertID() ) {
                insert = true;
                localFile = dirIter.next();
            }

            if( insert ) {
                hashAndEnqueueFile( localFile );
            }
        }
    }

    else {
        while( dirIter.hasNext() ) {
            auto localFile = dirIter.next();
            hashAndEnqueueFile( localFile );
        }
    }

    if( getResumeDirectory().isEmpty() ) {
        setResumeDirectory( path );
    }

    if( autoStart ) {
        emit started();
        prepareGameData( mFileInfoQueue );
        emit finished();
    }

    return 0;

}

void GameScannerOld::prepareGameData( QQueue<GameFileInfo> &queue ) {

    int i = 0;
    int queueLength = queue.size();

    qCDebug( phxLibrary ) << "Matching" << queue.size() << "file(s) against the database...";

    while( queue.size() > 0 && !insertCancelled() ) {
        ++i;

        while( insertPaused() ) {
            QThread::msleep( 500 );
            qCDebug( phxLibrary ) << "paused ";

            if( insertCancelled() ) {
                return;
            }
        }

        GameFileInfo gameInfo = queue.dequeue();

        // Find metadata
        gameInfo.scanOpenVGDBForGame();

        // qCDebug( phxLibrary ) << "ImportData: " << gameInfo.system() << gameInfo.fullFilePath() << gameInfo.artworkUrl();

        GameData importData;
        importData.importProgress = ( i / static_cast<qreal>( queueLength ) ) * 100.0;
        importData.timePlayed = gameInfo.timePlayed();
        importData.title = gameInfo.title();
        importData.filePath = gameInfo.fullFilePath();
        importData.crc32Checksum = gameInfo.crc32CheckSum();
        importData.system = gameInfo.system();
        importData.fileID = i - 1;
        importData.artworkUrl = gameInfo.artworkUrl();

        // Do not import any game that does not have a valid system set
        if( importData.system == "" ) {
            continue;
        }

        // Goodbyte data! Make us proud.
        emit insertGameData( std::move( importData ) );

    }

}

void GameScannerOld::hashAndEnqueueFile( QString filePath ) {

    auto fileInfo = GameFileInfo( filePath );

    switch( fileInfo.fileType() ) {
        case GameFileInfo::FileType::GameFile:
            mFileInfoQueue.enqueue( fileInfo );
            break;

        // If we encounter a .zip file, recurse into it as if it were a folder itself
        case GameFileInfo::FileType::ZipFile: {

            auto zipFileInfo = static_cast<ArchiveFileInfo>( fileInfo );

            if( zipFileInfo.open( QuaZip::mdUnzip ) ) {

                // Check all files within .zip
                for( auto hasNext = zipFileInfo.checkFirstFile(); hasNext; hasNext = zipFileInfo.checkNextFile() ) {
                    if( zipFileInfo.isValid() ) {
                        mFileInfoQueue.enqueue( zipFileInfo );
                    }
                }

                zipFileInfo.close();
            }

            break;

        }

        case GameFileInfo::FileType::CueFile: {
            auto cueFileInfo = static_cast<CueFileInfo>( fileInfo );

            if( cueFileInfo.isValid() ) {
                qCDebug( phxLibrary ) << "Cue File (Valid): " << cueFileInfo.fullFilePath() <<  cueFileInfo.system() << cueFileInfo.crc32CheckSum();
                mFileInfoQueue.enqueue( cueFileInfo );
            } else {
                qCDebug( phxLibrary ) << "Cue File (Invalid): " << cueFileInfo.fullFilePath();
            }

            break;
        }

        case GameFileInfo::FileType::BiosFile: {
            auto biosFileInfo = static_cast<BiosFileInfo>( fileInfo );
            biosFileInfo.cache( PhxPaths::firmwareLocation() );
            break;
        }

        default:
            break;
    }

}
