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

void GameHasher::setProgress( qreal progress ) {
    mTotalProgess = progress;

    if( static_cast<int>( progress ) % 5 == 0 || progress == 100.0 ) {
        emit progressChanged( mTotalProgess );
    }
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

            if( fileEntry.gameMetadata.romID == -1 ) {  // So we didn't find the game by its CRC32, now try to find it via the game's title, a.k.a. base name.
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

            openVGDBQuery.prepare( QStringLiteral( "SELECT RELEASES.releaseCoverFront, RELEASES.releaseTitleName FROM ROMs "
                                                   "INNER JOIN SYSTEMS ON SYSTEMS.systemID = ROMs.systemID "
                                                   "INNER JOIN RELEASES ON RELEASES.romID = ROMs.romID "
                                                   "WHERE ROMs.romID = :romID" ) );

            openVGDBQuery.bindValue( QStringLiteral( ":romID" ), fileEntry.gameMetadata.romID );

            bool exec = openVGDBQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

            if( openVGDBQuery.first() ) {

                fileEntry.gameMetadata.frontArtwork = openVGDBQuery.value( 0 ).toString();
                fileEntry.gameMetadata.title = openVGDBQuery.value( 1 ).toString();

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


            LibretroDatabase libretroDatabase;

            QSqlQuery mLibretroQuery( libretroDatabase );
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
                fileEntry.scannerResult = fileEntry.systemUUIDs.size() == 1 ? GameScannerResult::SystemUUIDKnown : GameScannerResult::MultipleSystemUUIDs;
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

void GameHasher::addPath( QString path ) {
    BetterFutureWatcher *watcher = new BetterFutureWatcher( nullptr );
    QStringList dirs = QStringList( path );
    QFuture<FileList> future = QtConcurrent::mappedReduced<FileList, QStringList>( dirs, MapFunctor( MapFunctor::One ), ReduceFunctor( ReduceFunctor::One ) );

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepOneFinished );

    qCDebug( phxLibrary ) << "Began scan at" << QDateTime::currentDateTime();

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );
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

    connect( watcher, &BetterFutureWatcher::finished, this, &GameHasher::stepThreeFinished );

    watcher->setFuture( future, mWatcherList.size() );
    mWatcherList.append( watcher );

    // Adjust stored index for each item in the list that has been moved by this list manipulation
    for( BetterFutureWatcher *b : mWatcherList ) {
        b->adjustIndex( pivot );
    }
}

void GameHasher::stepThreeFinished( BetterFutureWatcher *betterWatcher ) {
    FileList fileList = betterWatcher->futureWatcher().result();

    int pivot = betterWatcher->listIndex();

    // Basic cleanup, do not call 'delete', use 'deleteLater';
    mWatcherList.removeAt( pivot );
    betterWatcher->deleteLater();

    mFilesProcessing += fileList.size();

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

    mFilesProcessing -= fileList.size();

    qCDebug( phxLibrary ) << "Scan complete, finished scan at" << QDateTime::currentDateTime();

    qCDebug( phxLibrary ) << "Results:";

    for( FileEntry entry : fileList ) {
        qCDebug( phxLibrary ) << entry;
    }

    qCDebug( phxLibrary ) << "End of list";

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
