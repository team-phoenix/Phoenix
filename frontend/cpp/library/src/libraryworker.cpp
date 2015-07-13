
#include "libraryworker.h"
#include "logging.h"
#include "platforms.h"

#include <QThread>
#include <QDir>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QSettings>
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

    mMetaDatabase.close();
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

    emit started();

    if( resumeDirectory().isEmpty() ) {
        setResumeDirectory( localUrl );
    }

    mMetaDatabase.open();
    prepareGameData( mFileInfoQueue );
    mMetaDatabase.close();
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

        auto fileInfo = queue.dequeue();

        auto extension = fileInfo.suffix();

        // QFileInfo::baseName() seems to split the absoluteFilePath based on periods '.'
        // This causes issue with some game names that use periods.
        auto title =  fileInfo.absoluteFilePath().remove(
                          fileInfo.canonicalPath() ).remove( 0, 1 ).remove( QStringLiteral( "." ) + extension );
        auto absoluteFilePath = fileInfo.canonicalFilePath();

        // We need to check for .cue files specifically, since these files are text files.
        if( extension == QStringLiteral( "cue" ) ) {
            if( !getCueFileInfo( fileInfo ) ) {
                // Can't import a wonky cue file. Just skip it.
                // We should be telling the use that this cue file has an error.
                qCWarning( phxLibrary ) << fileInfo.canonicalFilePath()
                                        << " is isn't a valid cue file. Skipping...";
                return;
            }
        }

        // ####################################################################
        //                             WARNING!!!
        // ####################################################################

        // Every call to fileInfo after this point isn't guaranteed to have the
        // same values.

        // Make copies of the fileInfo data before this point, unless you want the
        // cue file check to override them.

        auto system = platformMap.value( extension, "" );
        auto sha1 = getCheckSum( fileInfo.canonicalFilePath() );


        // System should only be empty on ambiguous files, such as ISO's and BINS.
        if( system.isEmpty() ) {

            checkHeaderOffsets( fileInfo, system );

            if( system.isEmpty() ) {
                qCWarning( phxLibrary ) << "The system is 'still' empty, for"
                                        << fileInfo.canonicalFilePath();

                qCWarning( phxLibrary ) << "this means we need to add"
                                        << "in better header and offset checking.";
            }
        }


        auto progress = ( i / static_cast<qreal>( queueLength ) ) * 100.0;

        GameData importData;
        importData.timePlayed = QStringLiteral( "00:00" );
        importData.title = title;
        importData.filePath = absoluteFilePath;
        importData.sha1 = sha1;
        importData.importProgress = progress;
        importData.system = system;
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

    QSqlQuery query( mMetaDatabase.database() );

    query.prepare( romIDFetchStatement );
    query.addBindValue( gameData.sha1 );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Metadata select error: "
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
            qCWarning( phxLibrary ) << "Metadata select error: "
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

QString LibraryWorker::getCheckSum( const QString filePath ) {
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

bool LibraryWorker::getCueFileInfo( QFileInfo &fileInfo ) {
    QFile file( fileInfo.canonicalFilePath() );

    if( !file.open( QIODevice::ReadOnly ) ) {
        return false;
    }

    while( !file.atEnd() ) {
        auto line = file.readLine();
        QList<QByteArray> splitLine = line.split( ' ' );

        if( splitLine.first().toUpper() == "FILE" ) {
            QString baseName;

            for( int i = 1; i < splitLine.size() - 1; ++i ) {

                auto bytes = splitLine.at( i );

                bytes = bytes.replace( "\"", "" );

                if( i == splitLine.size() - 2 ) {
                    baseName += bytes;
                } else {
                    baseName += bytes + ' ';
                }
            }

            if( baseName.isEmpty() ) {
                return false;
            }

            fileInfo.setFile( fileInfo.canonicalPath() + QDir::separator() + baseName );
            break;

        }
    }

    file.close();

    return true;
}

void LibraryWorker::checkHeaderOffsets( const QFileInfo &fileInfo, QString &platform ) {
    QFile file( fileInfo.canonicalFilePath() );

    if( file.open( QIODevice::ReadOnly ) ) {

        auto headers = headerOffsets.value( fileInfo.suffix() );

        for( auto &header : headers ) {
            if( !file.seek( header.offset ) ) {
                continue;
            }

            auto bytes = file.read( header.length );

            platform = platformForHeaderString( bytes.simplified().toHex() );

            if( !platform.isEmpty() ) {
                break;
            }

        }

        file.close();
    }
}
