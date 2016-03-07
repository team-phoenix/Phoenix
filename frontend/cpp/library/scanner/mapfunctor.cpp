#include "mapfunctor.h"
#include "archivefile.h"
#include "cuefile.h"
#include "cryptohash.h"

#include <QFileInfo>
#include <QDirIterator>

#include "libretrodatabase.h"
#include "metadatadatabase.h"

using namespace Library;

MapFunctor::MapFunctor( const Step step )
    : mStep( step ) {

}

bool MapFunctor::searchDatabase( const SearchReason reason, FileEntry &fileEntry ) {

    switch( reason ) {

        case GetROMID: {
            // Step 1. Search the OpenVGDB, by CRC32 checksum, for the romID of the game.

            QSqlQuery openVGDBQuery( QSqlDatabase::database( QThread::currentThread()->objectName() ) );

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

            QSqlQuery openVGDBQuery( QSqlDatabase::database( QThread::currentThread()->objectName() ) );

            openVGDBQuery.prepare( QStringLiteral( "SELECT RELEASES.releaseCoverFront, RELEASES.releaseTitleName, SYSTEMS.systemName FROM ROMs "
                                                   "INNER JOIN SYSTEMS ON SYSTEMS.systemID = ROMs.systemID "
                                                   "INNER JOIN RELEASES ON RELEASES.romID = ROMs.romID "
                                                   "WHERE ROMs.romID = :romID" ) );

            openVGDBQuery.bindValue( QStringLiteral( ":romID" ), fileEntry.gameMetadata.romID );

            bool exec = openVGDBQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

            if( openVGDBQuery.first() ) {

                fileEntry.gameMetadata.frontArtwork = openVGDBQuery.value( 0 ).toString();
                fileEntry.gameMetadata.title = openVGDBQuery.value( 1 ).toString();
                fileEntry.gameMetadata.openVGDBsystemName = openVGDBQuery.value( 2 ).toString();

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


            //LibretroDatabase libretroDatabase;

            QSqlQuery mLibretroQuery( QSqlDatabase::database( QThread::currentThread()->objectName() ) );
            mLibretroQuery.prepare( QStringLiteral( "SELECT UUID, enabled FROM system "
                                                    "WHERE openvgdbSystemName = :openvgdbSystemName" ) );

            qDebug() << "systemName: " << fileEntry.gameMetadata.openVGDBsystemName;

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
            bool c = searchDatabase( GetArtwork, fileEntry );
            bool b = searchDatabase( GetSystemUUID, fileEntry );
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

FileList MapFunctor::operator()( const FileEntry &entry ) {
    FileList resultList;

    switch( mStep ) {
        case Two: {
            setBackgroundIOPriority();

            QFileInfo info( entry.filePath );

            if( info.suffix() == QStringLiteral( "zip" ) ) {
                // Expand and explore the zip file.
                ArchiveFile::ParseData parsedData = ArchiveFile::parse( entry.filePath );

                for( QString &file : parsedData.enumeratedFiles ) {
                    FileEntry entry;
                    entry.filePath = file;
                    resultList.append( entry );
                }

            } else {
                resultList.append( entry );
            }

            setNormalIOPriority();

            break;
        }

        case Three: {
            setBackgroundIOPriority();

            QFileInfo info( entry.filePath );

            resultList.append( entry );

            // If we've found a .cue file, enumerate all the .bin files within and add their paths to the result list
            if( info.suffix() == QStringLiteral( "cue" ) ) {
                // qDebug() << "Found cue file: " << entry.filePath;
                QStringList binFiles = CueFile::parse( entry.filePath );

                for( QString binFile : binFiles ) {
                    FileEntry newEntry;
                    newEntry.filePath = binFile;
                    newEntry.scannerResult = PartOfCueFile;
                    resultList.append( newEntry );
                }
            }

            setNormalIOPriority();

            break;
        }

        case Four: {
            // Just go out and fill all the data in at once
            // If this returns false, then that means one of the ops failed
            // I'm not sure which one, and at this stage I don't care

            setBackgroundIOPriority();

            FileEntry entryCopy = entry;

            // Each thread must have its own unique SQL connection
            // Give the thread a name to mark it as already having an SQL connection
            // Reuse this thread name for the actual SQL connection's name
            // FIXME: Connections are never closed
            QThread *thread = QThread::currentThread();

            if( !thread->objectName().startsWith( "MapReduce step 4 thread #" ) ) {
                static int i = 0;

                static QMutex mutex;
                mutex.lock();
                i++;

                const QString name( "MapReduce step 4 thread #" );

                thread->setObjectName( name + QString::number( i ) );

                MetaDataDatabase::addConnection( thread->objectName() );
                LibretroDatabase::addConnection( thread->objectName() );

                QSqlDatabase libretroDb = QSqlDatabase::database( thread->objectName() );
                QSqlDatabase openvgDb = QSqlDatabase::database( thread->objectName() );
                bool open1 = openvgDb.open();
                bool open2 = libretroDb.open();

                Q_ASSERT( open1 );
                Q_ASSERT( open2 );

                mutex.unlock();
            }

            // Hash file
            CryptoHash crc32Hash( CryptoHash::Crc32 );

            if( entryCopy.scannerResult != PartOfCueFile && crc32Hash.addData( entryCopy.filePath ) ) {
                entryCopy.hasHashCached = true;
                entryCopy.crc32 = crc32Hash.result();
            }

            // Search the database for this hash
            // TODO: Fall back to more broad database search stratagies as laid out in the header for step 4
            bool a = searchDatabase( GetROMID, entryCopy );
            bool c = searchDatabase( GetArtwork, entryCopy );
            Q_UNUSED( a )
            Q_UNUSED( c )

            //qDebug() << entryCopy.gameMetadata.frontArtwork;

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
    FileList resultList;

    QFileInfo dir( path );

    qCDebug( phxLibrary ) << "path: " << path;

    if( dir.isFile() ) {
        FileEntry entry;
        entry.filePath = path;
        resultList.append( entry );
    } else {
        // Path is a file system directory past this point
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
                resultList.append( entry );
            }
        }
    }

    return resultList;
}

QString MapFunctor::getLastExecutedQuery( const QSqlQuery &query ) {
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
