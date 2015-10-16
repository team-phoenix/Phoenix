#include "gamefileinfo.h"

using namespace Library;

GameFileInfo::GameFileInfo( const QString &file )
    : QFileInfo( file ),
      mFileType( FileType::UnsupportedFile ),
      mTitle( canonicalFilePath().remove( canonicalPath() ).remove( 0, 1 ).remove( QStringLiteral( "." ) + suffix() ) ),
      mFullFilePath( canonicalFilePath() ),
      mQuery( QSqlQuery( SystemDatabase::database() ) ),
      mTimePlayed( QStringLiteral( "00:00" ) )
{

    if ( QStringLiteral( "zip" ) == suffix() ) {
        mFileType = FileType::ZipFile;
    } else if ( QStringLiteral( "cue" ) == suffix() ) {
        mFileType = FileType::CueFile;
    } else if ( QStringLiteral( "bin" ) == suffix() ) {

        QString biosName;
        if ( isBios( biosName ) ) {
            mFileType = FileType::BiosFile;
            mTitle = biosName;
        }

    } else if ( QStringLiteral( "7z" ) == suffix() ) {

    } else if ( QStringLiteral( "iso" ) == suffix() ) {

    } else {
        mFileType = FileType::GameFile;
        update( suffix() );
    }
}

QString GameFileInfo::system() const
{
    return mSystem;
}

QString GameFileInfo::sha1CheckSum() const
{
    return mSha1Sum;
}

QString GameFileInfo::title() const
{
    return mTitle;
}

QString GameFileInfo::fullFilePath() const
{
    return mFullFilePath;
}

QString GameFileInfo::timePlayed() const
{
    return mTimePlayed;
}

QString GameFileInfo::crc32Checksum() const
{
    return mCrc32Checksum;
}

QString GameFileInfo::artworkUrl() const
{
    return mArtworkUrl;
}

GameFileInfo::FileType GameFileInfo::fileType() const
{
    return mFileType;
}

QStringList GameFileInfo::gameFilter() {
    static QStringList filter;

    if ( filter.isEmpty() ) {
        auto query = QSqlQuery( SystemDatabase::database() );
        auto exec = query.exec( QStringLiteral( "SELECT DISTINCT extension FROM extensions" ) );
        Q_ASSERT(  exec );

        while ( query.next() ) {
            auto extension = query.value( 0 ).toString();
            filter.append( QStringLiteral( "*." ) + extension );
        }
    }

    return filter;
}

QStringList GameFileInfo::getAvailableSystems(const QString &extension) {
    mQuery.prepare( QStringLiteral( "SELECT DISTINCT systemMap.systemname FROM systemMap" )
                                 + QStringLiteral( " INNER JOIN extensions ON systemMap.systemIndex=extensions.systemIndex" )
                                 + QStringLiteral( " WHERE extensions.extension = ?" ) );

    mQuery.addBindValue( extension );

    QStringList systemsList;

    if( mQuery.exec() ) {
        while( mQuery.next() ) {
            systemsList.append( mQuery.value( 0 ).toString() );
        }
    }

    return std::move( systemsList );
}

QString GameFileInfo::getRealSystem( const QList<GameFileInfo::HeaderData> &possibleHeaders ) {
    QFile gameFile( canonicalFilePath() );

    auto opened = gameFile.open( QIODevice::ReadOnly );
    Q_ASSERT( opened );

    QString realSystem;

    for( auto &headerData : possibleHeaders ) {

        if( !gameFile.seek( headerData.seekPosition ) ) {
            qCWarning( phxLibrary ) << "Could not put " << gameFile.fileName() << "'s seek at " << headerData.seekPosition;
            continue;
        }

        auto bytes = QString( gameFile.read( headerData.byteLength ).simplified().toHex() );

        if( bytes == headerData.result ) {

            mQuery.prepare( QStringLiteral( "SELECT systemname FROM systemMap WHERE systemIndex = ?" ) );
            mQuery.addBindValue( headerData.systemIndex );
            auto exec = mQuery.exec();

            Q_ASSERT( exec );

            mQuery.first();
            realSystem = mQuery.value( 0 ).toString();
            break;
        }

    }

    return std::move( realSystem );
}

void GameFileInfo::cache( const QString &location ) {

    auto file = location + mTitle + QStringLiteral( "." ) + suffix();

    if( !QFile::exists( file ) ) {
        QFile::copy( canonicalFilePath(), file );
    }

}

QString GameFileInfo::getRealSystem(const QList<GameFileInfo::HeaderData> &possibleHeaders, const QString &localFile ) {
    QFile gameFile( localFile );
    auto opened = gameFile.open( QIODevice::ReadOnly );
    Q_ASSERT( opened );

    QString realSystem;

    auto query = QSqlQuery( SystemDatabase::database() );

    for( auto &headerData : possibleHeaders ) {

        if( !gameFile.seek( headerData.seekPosition ) ) {
            qCWarning( phxLibrary ) << "Could not put " << gameFile.fileName() << "'s seek at " << headerData.seekPosition;
            continue;
        }

        auto bytes = QString( gameFile.read( headerData.byteLength ).simplified().toHex() );

        if( bytes == headerData.result ) {

            query.prepare( QStringLiteral( "SELECT systemname FROM systemMap WHERE systemIndex = ?" ) );
            query.addBindValue( headerData.systemIndex );

            auto exec = query.exec();
            Q_ASSERT( exec );

            query.first();
            realSystem = query.value( 0 ).toString();
            break;
        }

    }

    return std::move( realSystem );
}

void GameFileInfo::prepareMetadata() {

    static const QString statement = QStringLiteral( "SELECT romID FROM " )
            + MetaDataDatabase::tableRoms
            + QStringLiteral( " WHERE romHashSHA1 = ?" );

    static const auto zipStatement = QStringLiteral( "SELECT romID FROM " )
                                  + MetaDataDatabase::tableRoms
                                  + QStringLiteral( " WHERE romHashCRC = ?" );

    QSqlQuery query( MetaDataDatabase::database() );

    if ( fileType() == FileType::ZipFile ) {
        query.prepare( zipStatement );
        query.addBindValue( mCrc32Checksum );
    } else if ( fileType() == FileType::GameFile ) {
        query.prepare( statement );
        query.addBindValue( mSha1Sum );
    }

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Metadata fetch romID error: "
                                << query.lastError().text() << query.executedQuery();
        return;
    }

    // Get all of the rowID's to lookup in the RELEASES table.
    if( query.first() ) {
        fillMetadata( query.value( 0 ).toInt(), query );
    }

}

void GameFileInfo::fillMetadata( int romID, QSqlQuery &query ) {

    if( romID > -1 ) {

        static const auto statement = QStringLiteral( "SELECT RELEASES.releaseCoverFront, SYSTEMS.systemName FROM ROMs INNER JOIN SYSTEMS ON SYSTEMS.systemID = ROMs.systemID INNER JOIN RELEASES ON RELEASES.romID = ROMs.romID WHERE ROMs.romID = ?" );

        query.prepare( statement );
        query.addBindValue( romID );

        if( !query.exec() ) {

            qCWarning( phxLibrary ) << "Metadata fetch artwork error: "
                                    << query.lastError().text() << query.isActive() << query.isValid();
            return;
        }

        if( query.first() ) {

            mArtworkUrl = query.value( 0 ).toString();

            if ( mSystem.isEmpty() ) {
                mSystem = query.value( 1 ).toString();
            }

        }
    }
}

QList<GameFileInfo::HeaderData> GameFileInfo::getPossibleHeaders( const QStringList &possibleSystems ) {

    QList<HeaderData> headerDataList;

    for( auto &system : possibleSystems ) {

        mQuery.clear();
        mQuery.prepare( QStringLiteral( "SELECT DISTINCT systemHeaderOffsets.byteLength, " )
                                     + QStringLiteral( "systemHeaderOffsets.seekIndex, systemHeaderOffsets.result, systemHeaderOffsets.systemIndex " )
                                     + QStringLiteral( "FROM systemHeaderOffsets INNER JOIN  systemMap ON " )
                                     + QStringLiteral( "systemMap.systemIndex=systemHeaderOffsets.systemIndex " )
                                     + QStringLiteral( "WHERE systemMap.systemname = ?" ) );
        mQuery.addBindValue( system );

        auto exec = mQuery.exec();
        Q_ASSERT( exec );

        while( mQuery.next() ) {

            HeaderData headerData;
            headerData.byteLength = mQuery.value( 0 ).toInt();
            headerData.seekPosition = mQuery.value( 1 ).toInt();
            headerData.result = mQuery.value( 2 ).toString();
            headerData.systemIndex = mQuery.value( 3 ).toString();

            headerDataList.append( std::move( headerData ) );
        }
    }

    return std::move( headerDataList );
}

QString GameFileInfo::getCheckSum( const QString &filePath ) {
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

void GameFileInfo::update( const QString &extension ) {
    auto possibleSystemsList = getAvailableSystems( extension );

    if ( possibleSystemsList.size() == 1 ) {
        mSystem = possibleSystemsList.at( 0 );
    } else {
        auto possibleHeaders = getPossibleHeaders( possibleSystemsList );
        mSystem = getRealSystem( possibleHeaders );
    }

    mFullFilePath = QStringLiteral( "file://" ) + canonicalFilePath();
    mSha1Sum = getCheckSum( canonicalFilePath() );
}

bool GameFileInfo::isBios( QString &biosName ) {

    static const QString statement = QStringLiteral( "SELECT biosFile FROM firmware WHERE sha1 = ?" );

    mQuery.prepare( statement );
    mQuery.addBindValue( mSha1Sum );

    auto exec = mQuery.exec();
    Q_ASSERT( exec );

    if ( mQuery.first() ) {
        biosName = mQuery.value( 0 ).toString();

        return !biosName.isEmpty();
    }

    return false;
}

