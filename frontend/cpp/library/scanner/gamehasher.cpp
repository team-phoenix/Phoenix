#include "gamehasher.h"
#include "cuefile.h"
#include "phxpaths.h"
#include "libretrodatabase.h"
#include "metadatadatabase.h"
#include "cryptohash.h"

#include <QtConcurrent>

using namespace Library;

GameHasher::GameHasher( QObject *parent )
    : QObject( parent ),
      mTotalProgess( 0.0 ),
      mFilesProcessing( 0 ) {

}

qreal GameHasher::progress() const {
    return mTotalProgess;
}

void GameHasher::setProgress( qreal progress ) {
    mTotalProgess = progress;
    emit progressChanged( mTotalProgess );
}

bool GameHasher::isBios( QFileInfo &info, QString &trueBiosName ) {

    static const QString statement = QStringLiteral( "SELECT biosFile FROM firmware WHERE sha1 = ?" );

    QSqlQuery mLibretroQuery = QSqlQuery( LibretroDatabase::database() );

    QFile file( info.canonicalFilePath() );

    // This file may fail to open if the file is in a zip file, or in a cue file.
    // We can skip cue files, but may need to open zip files and examine..
    // For now just assume it isnt a bios file.
    if( !file.open( QIODevice::ReadOnly ) ) {
        return false;
    }

    QCryptographicHash sha1( QCryptographicHash::Sha1 );

    sha1.addData( &file );
    auto sha1Result = sha1.result().toHex().toUpper();

    file.close();

    mLibretroQuery.prepare( statement );
    mLibretroQuery.addBindValue( sha1Result );

    auto exec = mLibretroQuery.exec();
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() ) );

    if( mLibretroQuery.first() ) {
        trueBiosName = mLibretroQuery.value( 0 ).toString();

        return !trueBiosName.isEmpty();
    }

    return false;
}

bool GameHasher::searchDatabase( const SearchReason reason, FileEntry &fileEntry ) {

    switch( reason ) {

        case GetROMID: {
            // Step 1. Search the OpenVGDB, by CRC32 checksum, for the romID of the game.

            QSqlQuery openVGDBQuery( MetaDataDatabase::database() );

            if( fileEntry.hasHashCached ) {

                openVGDBQuery.prepare( QStringLiteral( "SELECT romID FROM " )
                                       % MetaDataDatabase::tableRoms
                                       % QStringLiteral( " WHERE romHashCRC LIKE :mCrc32Checksum" ) );

                openVGDBQuery.bindValue( QStringLiteral( ":mCrc32Checksum" ), fileEntry.crc32 );

                bool exec = openVGDBQuery.exec();
                Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

                // If we found a match, ask OpenVGDB for the rest
                if( openVGDBQuery.first() ) {
                    fileEntry.gameMetadata.romID = openVGDBQuery.value( 0 ).toInt();
                }
            }

            if( fileEntry.gameMetadata.romID == -1 ) {  // So we didn't find the game by it's CRC32, now try to find it via the game's title, a.k.a. base name.
                QFileInfo file( fileEntry.filePath );
                QString filename = file.fileName();

                // Filename must be sanitized before being passed into an SQL query
                openVGDBQuery.prepare( QStringLiteral( "SELECT romID FROM " )
                                       % MetaDataDatabase::tableRoms
                                       % QStringLiteral( " WHERE romFileName = :romFileName" ) );

                openVGDBQuery.bindValue( QStringLiteral( ":romFileName" ), filename );

                bool exec = openVGDBQuery.exec();
                Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

                if( openVGDBQuery.first() ) {
                    fileEntry.gameMetadata.romID = openVGDBQuery.value( 0 ).toInt();
                    //fillMetadataFromOpenVGDB( openVGDBQuery.value( 0 ).toInt(), openVGDBQuery );
                }
            }

            return fileEntry.gameMetadata.romID != -1;

        }

        case GetArtwork: {
            // Get me some of dat artwork!
            // romID should be good here, if not try to find it in the database.

            //qDebug() << "Before UUID";
            if( fileEntry.gameMetadata.romID == -1 && searchDatabase( GetROMID, fileEntry ) ) {
                return false;
            }

            //qDebug() << "Get UUID";

            // So we got our romID, now onto getting the cover art.

            QSqlQuery openVGDBQuery( MetaDataDatabase::database() );

            openVGDBQuery.prepare( QStringLiteral( "SELECT RELEASES.releaseCoverFront FROM ROMs "
                                                   "INNER JOIN SYSTEMS ON SYSTEMS.systemID = ROMs.systemID "
                                                   "INNER JOIN RELEASES ON RELEASES.romID = ROMs.romID "
                                                   "WHERE ROMs.romID = :romID" ) );

            openVGDBQuery.bindValue( QStringLiteral( ":romID" ), fileEntry.gameMetadata.romID );

            bool exec = openVGDBQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

            if( openVGDBQuery.first() ) {

                fileEntry.gameMetadata.frontArtwork = openVGDBQuery.value( 0 ).toString();

            }

            // Give me my artwork! (...hopefully...)"
            return !fileEntry.gameMetadata.frontArtwork.isEmpty();
        }

        case GetSystemUUID: {
            // Get the Phoenix system UUID

            // If we can't find the romID, then we must search for the
            // system via headers and fuzzy title matching.
            if( fileEntry.gameMetadata.romID == -1 && !searchDatabase( GetROMID, fileEntry ) ) {
                return false;
            }



            QSqlQuery mLibretroQuery( LibretroDatabase::database() );
            mLibretroQuery.prepare( QStringLiteral( "SELECT UUID, enabled FROM system "
                                                    "WHERE openvgdbSystemName=:openvgdbSystemName" ) );
            mLibretroQuery.bindValue( ":openvgdbSystemName", fileEntry.gameMetadata.openVGDBsystemName );

            bool exec = mLibretroQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() % " -- " % mLibretroQuery.lastQuery() ) );

            mLibretroQuery.first();

            while( mLibretroQuery.next() ) {

                // Do not import games for systems that are disabled
                // Leaving system name blank will make game scanner skip it

                fileEntry.systemUUIDs.append( mLibretroQuery.value( 0 ).toString() );

            }

            if( fileEntry.systemUUIDs.size() == 0 ) {
                fileEntry.scannerResult = GameScannerResult::SystemUUIDUnknown;

            } else {
                fileEntry.scannerResult = fileEntry.systemUUIDs.size() == 1 ? SystemUUIDKnown : MultipleSystemUUIDs;
            }


            return !fileEntry.systemUUIDs.isEmpty();
        }

        case GetMetadata: {
            bool a = searchDatabase( GetROMID, fileEntry );
            bool b = searchDatabase( GetSystemUUID, fileEntry );
            bool c = searchDatabase( GetArtwork, fileEntry );
            return ( a && b && c );
        }

        case GetHeaders: {
            //if ( fileEntry.scannerResult == GameScannerResult::)
            /*
            HeaderData headerData;
            for( auto &system : possibleSystems ) {
                static QString statement = QStringLiteral( "SELECT DISTINCT header.byteLength, header.seekIndex, header.result,"
                                                    " header.system FROM header INNER JOIN system ON system.UUID=\'%1\'"
                                                    " WHERE system.UUID=header.system AND system.enabled=1" );
                statement = statement.arg( system );
                bool exec = mLibretroQuery.exec( statement );
                Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() % mLibretroQuery.lastQuery() ) );
                if( mLibretroQuery.first() ) {
                    headerData.byteLength = mLibretroQuery.value( 0 ).toInt();
                    headerData.seekPosition = mLibretroQuery.value( 1 ).toInt();
                    headerData.result = mLibretroQuery.value( 2 ).toString();
                    headerData.system = system;
                } else {
                    // qCDebug( phxLibrary ) << "\n" << "Statement: " << statement << system << "\n" << mLibretroQuery.result();
                }
            }
            return headerData;
            */
            break;
        }

        default:
            Q_UNREACHABLE();
            break;
    }

    return false;
}

FileList GameHasher::stepOneMap( const QString &path ) {

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

void GameHasher::stepOneReduce( FileList &mergedList, const FileList &givenList ) {
    if( !givenList.isEmpty() ) {
        mergedList.append( givenList );
    }
}

FileList GameHasher::stepTwoMap( const FileEntry &filePath ) {
    FileList filePathList;

    QFileInfo info( filePath.filePath );

    if( info.suffix() == QStringLiteral( "zip" ) ) {
        // Expand and explore the zip file.

        ArchiveFile::ParseData parsedData = ArchiveFile::parse( filePath.filePath );

        for( QString &file : parsedData.enumeratedFiles ) {
            FileEntry entry;
            entry.filePath = file;
            filePathList.append( entry );

        }

    } else {
        filePathList.append( filePath );
    }


    return filePathList;
}

void GameHasher::stepTwoReduce( FileList &mergedList, const FileList &givenList ) {
    mergedList.append( givenList );
}

FileList GameHasher::stepThreeMap( const FileEntry &entry )  {
    FileList filePathList;

    QFileInfo info( entry.filePath );

    if( info.suffix() == QStringLiteral( "cue" ) ) {
        // Explore the cue file.
        qDebug() << "Found cue file: " << entry.filePath;
        QStringList binFiles = CueFile::parse( entry.filePath );

        for( QString binFile : binFiles ) {
            FileEntry newEntry;
            newEntry.filePath = binFile;
            filePathList.append( newEntry );
        }
    } else {
        filePathList.append( entry );
    }

    return filePathList;
}

void GameHasher::stepThreeReduce( FileList &mergedList, const FileList &givenList ) {
    mergedList.append( givenList );
}

bool GameHasher::stepFourFilter( const FileEntry &fileEntry ) {
    QFileInfo info( fileEntry.filePath );

    if( info.suffix() == QStringLiteral( "bin" ) ) {
        // Check for bios, cache if bios is found

        /*
        QString biosName;
        if ( GameScanner::isBios( info, biosName ) ) {
            qDebug() << "is an actual bios file";
            QString file = PhxPaths::firmwareLocation() + biosName + QStringLiteral( "." ) + info.suffix();
            qDebug() << "File To cache: " << file;
            //if( !QFile::exists( file ) ) {
                //QFile::copy( canonicalFilePath(), file );
            //}
        }
        */

        return false;
    }

    return true;
}

void GameHasher::stepFourFilterReduce( FileList &mergedList, const FileEntry &givenList ) {
    mergedList.append( givenList );
}

FileList GameHasher::stepFourMap( const FileEntry &fileEntry ) {
    // Just go out and fill me all the data in at once.
    // If this returns false, then that means one of the ops failed.
    // I'm not sure which one, and at this stage I don't care.

    FileEntry entry = fileEntry;

    // Hash file
    CryptoHash crc32Hash( CryptoHash::Crc32 );

    if( crc32Hash.addData( entry.filePath ) ) {
        entry.hasHashCached = true;
        entry.crc32 = crc32Hash.result();
    }

    FileList list;
    list.append( entry );

    return list;
}

void GameHasher::stepFourReduce( FileList &mergedList, const FileList &givenEntry ) {
    mergedList.append( givenEntry );
}

void GameHasher::addPath( QString path ) {
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QStringList dirs = QStringList( path );
    QFuture<FileList> future = QtConcurrent::mappedReduced( dirs, GameHasher::stepOneMap, GameHasher::stepOneReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepOneFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::stepOneFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    qDebug() << "Step 1 finished. " << fileList.size();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    // No point in starting for an empty list. Abort!!!
    if( fileList.isEmpty() ) {
        return;
    }

    // Start for step two
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced( fileList, GameHasher::stepTwoMap, GameHasher::stepTwoReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepTwoFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::stepTwoFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    qDebug() << "Step two finished." << fileList.size();

    // Start for step three
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced( fileList, GameHasher::stepThreeMap, GameHasher::stepThreeReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepThreeFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::stepThreeFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    qDebug() << "Step three finished. " << fileList.size();

    // Start for step four, filterReduce.
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::filteredReduced( fileList, GameHasher::stepFourFilter, GameHasher::stepFourFilterReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepFourFilterFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::stepFourFilterFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    mFilesProcessing += fileList.size();

    qDebug() << "Step four filter finished: " << fileList.size();

    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QFuture<FileList> future = QtConcurrent::mappedReduced( fileList, GameHasher::stepFourMap, GameHasher::stepFourReduce );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepFourMapReduceFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
}

void GameHasher::stepFourMapReduceFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();
    qDebug() << "Step four map reduce finished. " << fileList.size();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( betterWatcher->listIndex() );
    betterWatcher->deleteLater();

    // step four being finished via standard iteration.

    MetaDataDatabase::open();

    int i = 0;

    for( FileEntry &entry : fileList ) {
        searchDatabase( GetMetadata, entry );
        ++i;

        setProgress( ( i / static_cast<qreal>( mFilesProcessing ) ) * 100.0 );

        qDebug() << progress() << " : " << entry.filePath;

        // Don't block the thread completely.
        QCoreApplication::processEvents( QEventLoop::AllEvents );
    }

    MetaDataDatabase::close();

    mFilesProcessing -= fileList.size();

    qDebug() << "Finish Step 4 map reduce.";
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
