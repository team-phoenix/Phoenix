#include "mapfunctor.h"

using namespace Library;

MapFunctor::MapFunctor( const Step step )
    : mStep( step ) {

}

bool MapFunctor::searchDatabase( const SearchReason reason, FileEntry &fileEntry ) {
    switch( reason ) {
        case SearchReason::GetROMIDByHash: {
            QSqlQuery openVGDBQuery( QSqlDatabase::database( QThread::currentThread()->objectName() % "openvgdb" ) );

            Q_ASSERT( fileEntry.hasHashCached );

            openVGDBQuery.prepare( QStringLiteral( "SELECT romID FROM ROMs " )
                                   % QStringLiteral( "WHERE romHashCRC LIKE :mCrc32Checksum" ) );

            openVGDBQuery.bindValue( QStringLiteral( ":mCrc32Checksum" ), fileEntry.crc32 );

            bool exec = openVGDBQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

            if( openVGDBQuery.first() ) {
                fileEntry.gameMetadata.romID = openVGDBQuery.value( 0 ).toInt();
                return true;
            }

            break;
        }

        case SearchReason::GetROMIDByFilename: {
            QSqlQuery openVGDBQuery( QSqlDatabase::database( QThread::currentThread()->objectName() % "openvgdb" ) );
            QFileInfo file( GameLauncher::trimmedGameNoExtract( fileEntry.filePath ) );
            QString filename = file.fileName();

            // Filename must be sanitized before being passed into an SQL query
            openVGDBQuery.prepare( QStringLiteral( "SELECT romID FROM ROMs " )
                                   % QStringLiteral( "WHERE romFileName = :romFileName" ) );

            openVGDBQuery.bindValue( QStringLiteral( ":romFileName" ), filename );

            bool exec = openVGDBQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

            if( openVGDBQuery.first() ) {
                fileEntry.gameMetadata.romID = openVGDBQuery.value( 0 ).toInt();
                return true;
            }

            break;
        }

        case SearchReason::GetMetadata: {
            Q_ASSERT( fileEntry.gameMetadata.romID != -1 );

            QSqlQuery openVGDBQuery( QSqlDatabase::database( QThread::currentThread()->objectName() % "openvgdb" ) );

            openVGDBQuery.prepare( QStringLiteral( "SELECT RELEASES.releaseCoverFront, RELEASES.releaseTitleName, SYSTEMS.systemName, "
                                                   "REGIONS.regionName, RELEASES.releaseDeveloper, RELEASES.releaseDate, "
                                                   "RELEASES.releaseGenre, RELEASES.releaseDescription "
                                                   "FROM ROMs "
                                                   "INNER JOIN SYSTEMS ON SYSTEMS.systemID = ROMs.systemID "
                                                   "INNER JOIN RELEASES ON RELEASES.romID = ROMs.romID "
                                                   "INNER JOIN REGIONS ON ROMs.regionID = REGIONS.regionID "
                                                   "WHERE ROMs.romID = :romID " ) );

            openVGDBQuery.bindValue( QStringLiteral( ":romID" ), fileEntry.gameMetadata.romID );

            bool exec = openVGDBQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

            if( openVGDBQuery.first() ) {
                fileEntry.gameMetadata.frontArtwork = openVGDBQuery.value( 0 ).toString();
                fileEntry.gameMetadata.title = openVGDBQuery.value( 1 ).toString();
                fileEntry.gameMetadata.openVGDBSystemUUID = openVGDBQuery.value( 2 ).toString();
                fileEntry.gameMetadata.region = openVGDBQuery.value( 3 ).toString();
                fileEntry.gameMetadata.developer = openVGDBQuery.value( 4 ).toString();
                fileEntry.gameMetadata.releaseDate = openVGDBQuery.value( 5 ).toString();
                fileEntry.gameMetadata.genre = openVGDBQuery.value( 6 ).toString();

                // Unescape HTML from description
                QString html = openVGDBQuery.value( 7 ).toString();
                QTextDocument text;
                text.setHtml( html );
                fileEntry.gameMetadata.description = text.toPlainText();
                return true;
            }

            break;
        }

        case SearchReason::GetSystemUUID: {
            QSqlQuery libretroQuery( QSqlDatabase::database( QThread::currentThread()->objectName() % "libretro" ) );

            libretroQuery.prepare( QString( "SELECT systemUUID FROM openVGDBToSystem "
                                            "INNER JOIN system ON system.UUID = openVGDBToSystem.systemUUID "
                                            "WHERE openVGDBToSystem.openvgdbSystemName = :openvgdbSystemName "
                                            "AND system.enabled = 1 " ) );

            libretroQuery.bindValue( QStringLiteral( ":openvgdbSystemName" ), fileEntry.gameMetadata.openVGDBSystemUUID );

            bool exec = libretroQuery.exec();

            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( libretroQuery.lastError().text() % " -- " % libretroQuery.lastQuery() ) );

            while( libretroQuery.next() ) {
                fileEntry.systemUUIDs.append( libretroQuery.value( 0 ).toString() );
            }

            // Don't say anything about system UUIDs if we already have the game UUID
            if( !( fileEntry.scannerResult == GameScannerResult::GameUUIDByFilename
                   || fileEntry.scannerResult == GameScannerResult::GameUUIDByHash ) ) {
                if( fileEntry.systemUUIDs.isEmpty() ) {
                    fileEntry.scannerResult = GameScannerResult::SystemUUIDUnknown;
                } else {
                    fileEntry.scannerResult = fileEntry.systemUUIDs.size() == 1 ?
                                              GameScannerResult::SystemUUIDKnown : GameScannerResult::MultipleSystemUUIDs;
                }
            }

            break;
        }

        case SearchReason::GetSystemByHeader: {
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

        case SearchReason::GetSystemByExtension: {
            QString filePath = GameLauncher::trimmedGameNoExtract( fileEntry.filePath );
            QFileInfo fileInfo( filePath );
            QString extension = fileInfo.suffix();

            QSqlQuery libretroQuery( QSqlDatabase::database( QThread::currentThread()->objectName() % "libretro" ) );

            libretroQuery.prepare( QString( "SELECT system FROM extension "
                                            "INNER JOIN system ON system.UUID = extension.system "
                                            "WHERE extension.extension = :extension "
                                            "AND system.enabled = 1 " ) );

            libretroQuery.bindValue( QStringLiteral( ":extension" ), extension );

            bool exec = libretroQuery.exec();

            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( libretroQuery.lastError().text() % " -- " % libretroQuery.lastQuery() ) );

            while( libretroQuery.next() ) {
                fileEntry.systemUUIDs.append( libretroQuery.value( 0 ).toString() );
            }

            return ( !( fileEntry.systemUUIDs.isEmpty() ) );
        }

        case SearchReason::GetTitleByFilename: {
            QString filePath = GameLauncher::trimmedGameNoExtract( fileEntry.filePath );
            QFileInfo fileInfo( filePath );
            QString basename = fileInfo.completeBaseName();
            fileEntry.gameMetadata.title = basename;
            break;
        }

        default:
            Q_UNREACHABLE();
            break;
    }

    return false;
}

FileList MapFunctor::operator()( const FileEntry &entry ) {
    FileList resultList;

    switch( mStep ) {
        case Step::Two: {
            setBackgroundIOPriority();

            QFileInfo info( entry.filePath );

            if( info.suffix() == QStringLiteral( "zip" ) ) {
                // Expand and explore the zip file.
                ArchiveFile::ParseData parsedData = ArchiveFile::parse( entry.filePath );

                for( QString file : parsedData.fileHashesMap.keys() ) {
                    FileEntry entry;
                    entry.filePath = file;
                    entry.crc32 = parsedData.fileHashesMap[ file ];
                    Q_ASSERT( parsedData.fileHashesMap.contains( file ) );

                    if( !entry.crc32.isEmpty() ) {
                        entry.hasHashCached = true;
                    }

                    // qCDebug( phxLibrary ) << "Found file within zip:" << entry.filePath << entry.crc32;
                    resultList.append( entry );
                }

            } else {
                resultList.append( entry );
            }

            setNormalIOPriority();

            break;
        }

        case Step::Three: {
            setBackgroundIOPriority();

            // If we've found a .cue file, enumerate all the .bin files within and add their paths to the result list
            if( entry.filePath.endsWith( QStringLiteral( "cue" ), Qt::CaseInsensitive ) ) {
                // qDebug() << "Found cue file: " << entry.filePath;
                QStringList binFiles = CueFile::parse( entry.filePath );

                for( QString binFile : binFiles ) {
                    FileEntry newEntry;
                    newEntry.filePath = binFile;
                    newEntry.scannerResult = GameScannerResult::PartOfCueFile;
                    resultList.append( newEntry );
                }
            }

            setNormalIOPriority();

            break;
        }

        case Step::Four: {
            setBackgroundIOPriority();

            FileEntry entryCopy = entry;

            // Each thread must have its own unique SQL connection
            // Give the thread a name to mark it as already having an SQL connection
            // Reuse this thread name for the actual SQL connection's name
            // FIXME: Connections are never closed
            QThread *thread = QThread::currentThread();

            if( !thread->objectName().startsWith( QStringLiteral( "MapReduce step 4 thread #" ) ) ) {
                static int i = 0;

                static QMutex mutex;
                mutex.lock();
                i++;

                const QString name = QStringLiteral( "MapReduce step 4 thread #" );

                thread->setObjectName( name % QString::number( i ) % QStringLiteral( " " ) );

                QSqlDatabase libretroDB = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), thread->objectName() % QStringLiteral( "libretro" ) );
                QSqlDatabase openVGDB = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), thread->objectName() % QStringLiteral( "openvgdb" ) );

                libretroDB.setDatabaseName( PhxPaths::metadataLocation() % QStringLiteral( "/libretro.sqlite" ) );
                openVGDB.setDatabaseName( PhxPaths::metadataLocation() % QStringLiteral( "/openvgdb.sqlite" ) );

                // Test the connection
                Q_ASSERT( libretroDB.open() );
                Q_ASSERT( openVGDB.open() );

                mutex.unlock();
            }

            // Hash file

            // TODO: Progress reporting (at least for large files > 16MB)
            if( !entryCopy.hasHashCached ) {
                CryptoHash crc32Hash( CryptoHash::Crc32 );
                crc32Hash.addData( entryCopy.filePath );
                entryCopy.crc32 = crc32Hash.result();
                entryCopy.hasHashCached = true;
            }

            // Search the database for this hash
            // A match by CRC32 or filename will get us a game UUID (and the system, too)
            // A match by header will give us one system
            // A match by extension will give us 1 or 2+ systems

            // Search by CRC32
            if( searchDatabase( SearchReason::GetROMIDByHash, entryCopy ) ) {
                searchDatabase( SearchReason::GetMetadata, entryCopy );
                searchDatabase( SearchReason::GetSystemUUID, entryCopy );
                entryCopy.scannerResult = GameScannerResult::GameUUIDByHash;
            }

            // Search by filename
            else if( searchDatabase( SearchReason::GetROMIDByFilename, entryCopy ) ) {
                searchDatabase( SearchReason::GetMetadata, entryCopy );
                searchDatabase( SearchReason::GetSystemUUID, entryCopy );
                entryCopy.scannerResult = GameScannerResult::GameUUIDByFilename;
            }

            // The best we can do past this point is assign it a system and title it after its filename

            // Search by headers if filename matches
            // TODO: Multiple systems for same header?
            else if( searchDatabase( SearchReason::GetSystemByHeader, entryCopy ) ) {
                // TODO: Maybe get title from header? Probably only worth it for newer disk-based games whose devs
                // cared about making the internal title look nice on-screen
                searchDatabase( SearchReason::GetTitleByFilename, entryCopy );
                entryCopy.scannerResult = GameScannerResult::SystemUUIDKnown;
            }

            // Search by extension
            else if( searchDatabase( SearchReason::GetSystemByExtension, entryCopy ) ) {
                searchDatabase( SearchReason::GetTitleByFilename, entryCopy );

                if( entryCopy.systemUUIDs.size() == 1 ) {
                    entryCopy.scannerResult = GameScannerResult::SystemUUIDKnown;
                } else {
                    entryCopy.scannerResult = GameScannerResult::MultipleSystemUUIDs;
                }
            }

            else {
                searchDatabase( SearchReason::GetTitleByFilename, entryCopy );
                entryCopy.scannerResult = GameScannerResult::SystemUUIDUnknown;
            }

            resultList.append( entryCopy );

            setNormalIOPriority();

            break;
        }

        default:
            break;
    }

    return resultList;
}

FileList MapFunctor::operator()( const QString &path ) {
    setBackgroundIOPriority();

    FileList resultList;

    QFileInfo dir( path );

    // qCDebug( phxLibrary ) << "path: " << path;

    if( dir.isFile() ) {
        FileEntry entry;
        entry.filePath = path;
        resultList.append( entry );
    } else if( dir.isDir() ) {
        // Path is a file system directory past this point
        QDir directory( path );

        if( !directory.exists() ) {
            return FileList();
        }

        QDirIterator dirIter( path, QDir::Files, QDirIterator::Subdirectories );

        while( dirIter.hasNext() ) {
            const QString filePath = dirIter.next();

            if( QFile::exists( filePath ) ) {
                FileEntry entry;
                entry.filePath = filePath;
                resultList.append( entry );
            }
        }
    }

    setNormalIOPriority();

    return resultList;
}
