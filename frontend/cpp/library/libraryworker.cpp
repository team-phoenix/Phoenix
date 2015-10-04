
#include "libraryworker.h"
#include "logging.h"
#include "platforms.h"
#include "phxpaths.h"

#include <QThread>
#include <QDir>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QMutexLocker>

using namespace Library;

LibraryWorker::LibraryWorker( QObject *parent )
    : QObject( parent ),
      mInsertCancelled( false ),
      mInsertPaused( false ),
      mRunning( false ),
      qmlResumeQuitScan( false ) {

    for( auto &extension : platformMap.keys() ) {
        mFileFilters.append( "*." + extension );
    }

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
                findGameFiles( resumeDirectory() );
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

        if ( QDir( localUrl ).exists() ) {
            findGameFiles( std::move( localUrl ) );

        }
        else {
            qDebug() << localUrl << "Is a file";
            mFileInfoQueue.enqueue( std::move( QFileInfo( localUrl ) ) );
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

void LibraryWorker::findGameFiles( const QString localUrl ) {

    QDir urlDirectory( localUrl );

    if( !urlDirectory.exists() ) {
        qCWarning( phxLibrary ) << localUrl << " does not exist!";
        return;
    }

    QDirIterator dirIter( localUrl, mFileFilters, QDir::Files, QDirIterator::NoIteratorFlags );

    if( resumeQuitScan() && !resumeInsertID().isEmpty() ) {

        bool insert = false;

        while( dirIter.hasNext() ) {
            auto dir = dirIter.next();

            if( dir == resumeInsertID() ) {
                insert = true;
                dir = dirIter.next();
            }

            if( insert ) {
                mFileInfoQueue.enqueue( std::move( QFileInfo( dir ) ) );
            }
        }
    }

    else {
        while( dirIter.hasNext() ) {
            mFileInfoQueue.enqueue( std::move( QFileInfo( dirIter.next() ) ) );
        }
    }

    if( resumeDirectory().isEmpty() ) {
        setResumeDirectory( localUrl );
    }

    emit started();
    prepareGameData( mFileInfoQueue );
    emit finished();

}

void LibraryWorker::prepareGameData( QQueue<QFileInfo> &queue ) {

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

        QFileInfo fileInfo = queue.dequeue();

        QString gameFilePath;
        QString gameExtension;
        QString gameTitle;
        QString gameSystem;
        QString gameSha1;

        gameFilePath = fileInfo.canonicalFilePath();
        gameExtension = fileInfo.suffix().toLower();

        // QFileInfo::baseName() seems to split the absoluteFilePath based on periods '.'
        // This causes issue with some game names that use periods.
        gameTitle =  fileInfo.absoluteFilePath().remove(
                          fileInfo.canonicalPath() ).remove( 0, 1 ).remove( QStringLiteral( "." ) + gameExtension );


        auto query = QSqlQuery( SystemDatabase::database() );

        GameData importData;

        auto possibleSystemsList = getAvailableSystems( gameExtension, query );

        if ( QStringLiteral( "cue" ) == gameExtension ) {
            CueData cueData = getCueData( possibleSystemsList, fileInfo, query );
            gameSystem = cueData.system;
            gameSha1 = cueData.sha1;
        } else if ( QStringLiteral( "bin" ) == gameExtension ) {
            auto hash = getCheckSum( fileInfo.canonicalFilePath() );
            if ( checkForBios( fileInfo.canonicalFilePath(), hash, query ) ) {
                continue;
            }

            gameSha1 = hash;

        } else {
            if ( possibleSystemsList.size() == 1 ) {
                gameSystem = possibleSystemsList.at( 0 );
            } else {
                auto possibleHeaders = getPossibleHeaders( possibleSystemsList, query );
                gameSystem = getRealSystem( possibleHeaders, fileInfo.canonicalFilePath(), query );
            }
            gameSha1 = getCheckSum( fileInfo.canonicalFilePath() );
        }

        if ( gameSystem.isEmpty() ) {
            qCDebug(phxLibrary)  << "The system could not be found, The database needs to be updated for " << fileInfo.canonicalFilePath();
            continue;
        }

        qDebug() << "GAME: " << gameSystem << gameTitle;

        importData.timePlayed = QStringLiteral( "00:00" );
        importData.title = gameTitle;
        importData.filePath = fileInfo.canonicalFilePath();
        importData.sha1 = gameSha1;
        importData.importProgress = ( i / static_cast<qreal>( queueLength ) ) * 100.0;
        importData.system = gameSystem;
        importData.fileID = i - 1;

        // Find MetaData.
        prepareMetadata( importData );

        // Goodbyte data! Make us proud.
        emit insertGameData( std::move( importData ) );


    }

}

void LibraryWorker::prepareMetadata( GameData &gameData ) {


    static const QString romIDFetchStatement = QStringLiteral( "SELECT romID FROM " )
            + MetaDataDatabase::tableRoms
            + QStringLiteral( " WHERE romHashSHA1 = ?" );

    QSqlQuery query( MetaDataDatabase::database() );

    query.prepare( romIDFetchStatement );
    query.addBindValue( gameData.sha1 );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Metadata fetch romID error: "
                                << query.lastError().text();
        return;
    }

    // Get all of the rowID's to lookup in the RELEASES table.
    auto romID = -1;

    if( query.first() ) {
        romID = query.value( 0 ).toInt();
    }

    gameData.updated = ( romID != -1 );

    if( gameData.updated ) {

        query.clear();

        static const QString artworkFetchStatement = QStringLiteral(
                    "SELECT releaseCoverFront, releaseDescription, releaseDeveloper, releaseGenre, releaseDate, regionLocalizedID " )
                    + QStringLiteral( " FROM " ) + MetaDataDatabase::tableReleases
                    + QStringLiteral( " WHERE romID = ?" );

        query.prepare( artworkFetchStatement );
        query.addBindValue( romID );

        if( !query.exec() ) {
            qCWarning( phxLibrary ) << "Metadata fetch artwork error: "
                                    << query.lastError().text();
            return;
        }

        if( query.first() ) {

            gameData.artworkUrl = query.value( 0 ).toString();
            gameData.description = query.value( 1 ).toString();
            gameData.developer = query.value( 2 ).toString();
            gameData.genre = query.value( 3 ).toString();
            gameData.releaseDate = query.value( 4 ).toString();
            gameData.region  = query.value( 5 ).toString();

        }
    }

}

QString LibraryWorker::getCheckSum( const QString &filePath ) {
    QString hash;
    QFile file( filePath );

    if( file.open( QIODevice::ReadOnly ) ) {

        QCryptographicHash checkSum( QCryptographicHash::Sha1 );
        checkSum.addData( &file );

        hash = checkSum.result().toHex().toUpper();

        file.close();
    }

    return std::move( hash );
}

bool LibraryWorker::checkForBios( const QString &filePath, const QString &checkSum, QSqlQuery &query ) {

    static const QString statement = QStringLiteral( "SELECT biosFile FROM firmware WHERE sha1 = ?" );

    query.prepare( statement );
    query.addBindValue( checkSum );

    Q_ASSERT( query.exec() );

    if ( query.first() ) {
        auto biosName = query.value(0).toString();
        if ( !biosName.isEmpty() ) {
            cacheBiosFile( filePath, biosName );
            return true;
        }
    }

    return false;
}

void LibraryWorker::cacheBiosFile( const QString &filePath, const QString &biosName ) {

    auto biosFile = PhxPaths::biosLocation() + biosName;

    if( !QFile::exists( biosFile ) ) {
        QFile::copy( filePath, biosFile );
    }

}

QStringList LibraryWorker::getAvailableSystems(const QString &extension, QSqlQuery &query )
{
    query.prepare( QStringLiteral( "SELECT DISTINCT systemMap.systemname FROM systemMap" )
                   + QStringLiteral( " INNER JOIN extensions ON systemMap.systemIndex=extensions.systemIndex" )
                   + QStringLiteral( " WHERE extensions.extension = ?" ) );

    query.addBindValue( extension );

    QStringList systemsList;
    if ( query.exec() ) {
        while( query.next() ) {
            systemsList.append( query.value(0).toString() );
        }
    }

    return std::move( systemsList );
}

QList<HeaderData> LibraryWorker::getPossibleHeaders( const QStringList &possibleSystems, QSqlQuery &query ) {

    QList<HeaderData> headerDataList;

    for ( auto &system : possibleSystems ) {

        query.clear();
        query.prepare( QStringLiteral( "SELECT DISTINCT systemHeaderOffsets.byteLength, ")
                       + QStringLiteral( "systemHeaderOffsets.seekIndex, systemHeaderOffsets.result, systemHeaderOffsets.systemIndex " )
                       + QStringLiteral( "FROM systemHeaderOffsets INNER JOIN  systemMap ON " )
                       + QStringLiteral( "systemMap.systemIndex=systemHeaderOffsets.systemIndex " )
                       + QStringLiteral( "WHERE systemMap.systemname = ?" ) );
        query.addBindValue( system );

        Q_ASSERT( query.exec() );

        while ( query.next() ) {

            HeaderData headerData;
            headerData.byteLength = query.value(0).toInt();
            headerData.seekPosition = query.value(1).toInt();
            headerData.result = query.value(2).toString();
            headerData.systemIndex = query.value(3).toString();

            headerDataList.append( std::move( headerData ) );
        }
    }

    return std::move( headerDataList );
}

QString LibraryWorker::getRealSystem(const QList<HeaderData> &possibleHeaders, const QString &gameFilePath, QSqlQuery &query)
{
    QFile gameFile( gameFilePath );

    Q_ASSERT( gameFile.open( QIODevice::ReadOnly ) );

    QString realSystem;

    for ( auto &headerData : possibleHeaders ) {
        if( !gameFile.seek( headerData.seekPosition ) ) {
            qCWarning(phxLibrary ) << "Could not put " << gameFile.fileName() << "'s seek at " << headerData.seekPosition;
            continue;
        }

        auto bytes = QString( gameFile.read( headerData.byteLength ).simplified().toHex() );

        if ( bytes == headerData.result ) {
            query.prepare( QStringLiteral( "SELECT systemname FROM systemMap WHERE systemIndex = ?" ) );
            query.addBindValue( headerData.systemIndex );

            Q_ASSERT( query.exec() );

            query.first();
            realSystem = query.value(0).toString();
            break;
        }

    }

    return std::move( realSystem );
}

CueData LibraryWorker::getCueData( const QStringList &possibleSystems, const QFileInfo &fileInfo, QSqlQuery &query )
{
    auto possibleHeaders = getPossibleHeaders( possibleSystems, query );

    QString firstCueBinaryFile;
    QStringList cueFileList = getCueFileInfo( fileInfo );

    if ( !cueFileList.isEmpty() ) {
        firstCueBinaryFile = cueFileList.first();
    }

    CueData cueData;
    cueData.system = getRealSystem( possibleHeaders, firstCueBinaryFile, query );
    cueData.sha1 = getCheckSum( firstCueBinaryFile );

    return std::move( cueData );
}

QStringList LibraryWorker::getCueFileInfo( const QFileInfo &fileInfo ) {
    QFile file( fileInfo.canonicalFilePath() );
    QStringList binFiles;

    if( !file.open( QIODevice::ReadOnly ) ) {
        return binFiles;
    }

    while( !file.atEnd() ) {
        auto line = file.readLine();

        QString obtainedTitle;
        bool eatChars = false;
        for ( auto &subStr : line ) {
            if ( subStr == '"' ) {
                if ( eatChars ) {
                    eatChars = false;
                } else {
                    eatChars = true;
                    if ( obtainedTitle.isEmpty() ) {
                        continue;
                    }
                }
            }

            if ( eatChars ) {
                obtainedTitle.append( subStr );
            }
        }

        if ( !obtainedTitle.isEmpty() ) {
            binFiles.append( fileInfo.canonicalPath() + QDir::separator() + obtainedTitle );
        }

    }

    file.close();

    return binFiles;
}

void LibraryWorker::checkHeaderOffsets( const QFileInfo &fileInfo, Platform::Platforms &platform ) {
    QFile file( fileInfo.canonicalFilePath() );

    if( file.open( QIODevice::ReadOnly ) ) {

        auto headers = headerOffsets.value( fileInfo.suffix() );

        for( auto &header : headers ) {
            if( !file.seek( header.offset ) ) {
                continue;
            }

            auto bytes = file.read( header.length );

            platform = Platform::checkHeaderString( bytes.simplified().toHex() );

            if( platform != Platform::Platform::UNKNOWN ) {
                break;
            }

        }

        file.close();
    }
}
