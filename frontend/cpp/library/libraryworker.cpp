#include "libraryworker.h"
#include "JlCompress.h"

#include "archivefileinfo.h"
#include "cuefileinfo.h"
#include "biosfileinfo.h"

using namespace Library;

LibraryWorker::LibraryWorker( QObject *parent )
    : QObject( parent ),
      mInsertCancelled( false ),
      mInsertPaused( false ),
      mRunning( false ),
      qmlResumeQuitScan( false ) {

    connect( this, &LibraryWorker::started, this, [ this ] {
        setIsRunning( true );
    } );

    connect( this, &LibraryWorker::finished, this, [ this ] {
        setIsRunning( false );
        mFileInfoQueue.clear();
        setInsertCancelled( false );
    } );

}

LibraryWorker::~LibraryWorker() {

    if( !resumeQuitScan() ) {
        setResumeDirectory( "" );
        setResumeInsertID( "" );
    }

    QSettings settings;
    settings.beginGroup( QStringLiteral( "Library" ) );
    settings.setValue( QStringLiteral( "ResumeInsertID" ), resumeInsertID() );
    settings.setValue( QStringLiteral( "ResumeDirectory" ), resumeDirectory() );

}

void LibraryWorker::eventLoopStarted() {

    if( resumeQuitScan() ) {
        QSettings settings;

        settings.beginGroup( QStringLiteral( "Library" ) );
        auto value = settings.value( QStringLiteral( "ResumeInsertID" ) );
        auto lastUsedPath = settings.value( QStringLiteral( "ResumeDirectory" ) );

        if( value.isValid() && lastUsedPath.isValid() ) {
            setResumeInsertID( value.toString() );
            setResumeDirectory( lastUsedPath.toString() );

            if( !resumeInsertID().isEmpty() ) {
                findGameFiles( resumeDirectory(), true );
            }
        }
    }
}

void LibraryWorker::handleDraggedUrls( QList<QUrl> urls ) {
    mDraggedUrls = urls;
}

void LibraryWorker::handleDroppedUrls() {

    for( auto &url : mDraggedUrls ) {

        auto localUrl = url.toLocalFile();

        if ( !findGameFiles( localUrl, false ) ) {
            enqueueFiles( localUrl );
        }

    }

    emit started();
    prepareGameData( mFileInfoQueue );
    emit finished();
}

void LibraryWorker::handleContainsDrag( const bool contains ) {
    if( !contains ) {
        mDraggedUrls.clear();
    }
}

bool LibraryWorker::resumeQuitScan() {
    QMutexLocker locker( &mMutex );
    return qmlResumeQuitScan;
}

void LibraryWorker::setResumeQuitScan( const bool resume ) {
    QMutexLocker locker( &mMutex );
    qmlResumeQuitScan = resume;
}

QString LibraryWorker::resumeDirectory() {
    QMutexLocker locker( &mMutex );
    return mResumeDirectory;;
}

void LibraryWorker::setResumeDirectory( const QString directory ) {
    QMutexLocker locker( &mMutex );
    mResumeDirectory = directory;
}

QString LibraryWorker::resumeInsertID() {
    QMutexLocker locker( &mMutex );
    return mResumeInsertID;
}

void LibraryWorker::setResumeInsertID( const QString id ) {
    QMutexLocker locker( &mMutex );
    mResumeInsertID = id;
}

bool LibraryWorker::isRunning() {
    QMutexLocker locker( &mMutex );
    return mRunning;
}

void LibraryWorker::setIsRunning( const bool running ) {
    QMutexLocker locker( &mMutex );
    mRunning = running;
}

bool LibraryWorker::insertCancelled() {
    QMutexLocker locker( &mMutex );
    return mInsertCancelled;
}

bool LibraryWorker::insertPaused() {
    QMutexLocker locker( &mMutex );
    return mInsertPaused;
}

void LibraryWorker::setInsertCancelled( const bool cancelled ) {
    QMutexLocker locker( &mMutex );

    if( cancelled ) {
        //mResumeDirectory.clear();
        //mResumeInsertID.clear();
    }

    mInsertCancelled = cancelled;
}

void LibraryWorker::setInsertPaused( const bool paused ) {
    QMutexLocker locker( &mMutex );
    mInsertPaused = paused;
}

bool LibraryWorker::findGameFiles( const QString localUrl, bool autoStart = true ) {

    QDir urlDirectory( localUrl );

    if( !urlDirectory.exists() ) {
        qCWarning( phxLibrary ) << localUrl << " does not exist!";
        return false;
    }

    QDirIterator dirIter( localUrl, GameFileInfo::gameFilter(), QDir::Files, QDirIterator::NoIteratorFlags );

    if( resumeQuitScan() && !resumeInsertID().isEmpty() ) {

        bool insert = false;

        while( dirIter.hasNext() ) {
            auto localFile = dirIter.next();

            if( localFile == resumeInsertID() ) {
                insert = true;
                localFile = dirIter.next();
            }

            if( insert ) {
                enqueueFiles( localFile );
            }
        }
    }

    else {
        while( dirIter.hasNext() ) {
            auto localFile = dirIter.next();
            enqueueFiles( localFile );
        }
    }

    if( resumeDirectory().isEmpty() ) {
        setResumeDirectory( localUrl );
    }

    if ( autoStart ) {
        emit started();
        prepareGameData( mFileInfoQueue );
        emit finished();
    }

    return true;

}

void LibraryWorker::prepareGameData( QQueue<GameFileInfo> &queue ) {

    int i = 0;
    int queueLength = queue.size();

    while( queue.size() > 0 && !insertCancelled() ) {
        ++i;

        while( insertPaused() ) {
            QThread::msleep( 500 );
            qDebug() << "paused ";

            if( insertCancelled() ) {
                return;
            }
        }

        GameFileInfo gameInfo = queue.dequeue();

        // Find MetaData.
        gameInfo.prepareMetadata();

        qDebug() << "ImportData: " << gameInfo.system() << gameInfo.fullFilePath() << gameInfo.artworkUrl();

        GameData importData;
        importData.importProgress = ( i / static_cast<qreal>( queueLength ) ) * 100.0;
        importData.timePlayed = gameInfo.timePlayed();
        importData.title = gameInfo.title();
        importData.filePath = gameInfo.fullFilePath();
        importData.sha1 = gameInfo.sha1CheckSum();
        importData.system = gameInfo.system();
        importData.fileID = i - 1;
        importData.artworkUrl = gameInfo.artworkUrl();

        // Goodbyte data! Make us proud.
        emit insertGameData( std::move( importData ) );

    }

}

void LibraryWorker::enqueueFiles( QString &filePath ) {

    auto fileInfo = GameFileInfo( filePath );

    switch( fileInfo.fileType() ) {
    case GameFileInfo::FileType::GameFile:
        mFileInfoQueue.enqueue( fileInfo );
        break;
    case GameFileInfo::FileType::ZipFile: {

        auto zipFileInfo = static_cast<ArchiveFileInfo>( fileInfo );

        if ( zipFileInfo.open( QuaZip::mdUnzip ) ) {

            for ( auto hasNext = zipFileInfo.firstFile(); hasNext; hasNext = zipFileInfo.nextFile() ) {
                if ( zipFileInfo.isValid() ) {
                    mFileInfoQueue.enqueue( zipFileInfo );
                }
            }

            zipFileInfo.close();
        }

        break;

    }
    case GameFileInfo::FileType::CueFile: {
        auto cueFileInfo = static_cast<CueFileInfo>( fileInfo );

        if ( cueFileInfo.isValid() ) {
            qDebug() << "Cue File (Valid): " << cueFileInfo.fullFilePath() <<  cueFileInfo.system() << cueFileInfo.sha1CheckSum();
            mFileInfoQueue.enqueue( cueFileInfo );
        } else {
            qDebug() << "Cue File (Invalid): " << cueFileInfo.fullFilePath();
        }
        break;
    }
    case GameFileInfo::FileType::BiosFile: {
        auto biosFileInfo = static_cast<BiosFileInfo>( fileInfo );
        biosFileInfo.cache( PhxPaths::biosLocation() );
        break;
    }
    default:
        break;
    }

}
