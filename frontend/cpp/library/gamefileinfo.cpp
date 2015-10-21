#include "gamefileinfo.h"
#include "cryptohash.h"

#include <QStringBuilder>

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

QString GameFileInfo::crc32CheckSum() const
{
    return mCrc32Checksum;
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
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

        while ( query.next() ) {
            auto extension = query.value( 0 ).toString();
            filter.append( QStringLiteral( "*." ) + extension );
        }
    }

    return filter;
}

QStringList GameFileInfo::getAvailableSystems(const QString &extension) {
    mQuery.prepare( QStringLiteral( "SELECT DISTINCT systems.phoenixSystemName FROM systems" )
                                 + QStringLiteral( " INNER JOIN extensions ON systems.infoSystemName=extensions.infoSystemName" )
                                 + QStringLiteral( " WHERE extensions.extension = ?" ) );

    mQuery.addBindValue( extension );

    QStringList systemsList;


    auto exec = mQuery.exec();
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mQuery.lastError().text() ) );

    while( mQuery.next() ) {
        systemsList.append( mQuery.value( 0 ).toString() );
    }

    return std::move( systemsList );
}

void GameFileInfo::cache( const QString &location ) {

    auto file = location + mTitle + QStringLiteral( "." ) + suffix();

    if( !QFile::exists( file ) ) {
        QFile::copy( canonicalFilePath(), file );
    }

}

void GameFileInfo::prepareMetadata() {

    static const QString statement = QStringLiteral( "SELECT romID FROM " )
            + MetaDataDatabase::tableRoms
            + QStringLiteral( " WHERE romHashCRC = ?" );

    QSqlQuery query( MetaDataDatabase::database() );

    query.prepare( statement );
    query.addBindValue( mCrc32Checksum );

    auto exec = query.exec();
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

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

        auto exec = query.exec();
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

        if( query.first() ) {

            mArtworkUrl = query.value( 0 ).toString();

            if ( mSystem.isEmpty() ) {
                mSystem = query.value( 1 ).toString();
            }

        }
    }
}

GameFileInfo::HeaderData GameFileInfo::getPossibleHeaders( const QStringList &possibleSystems ) {

    HeaderData headerData;

    for( auto &system : possibleSystems ) {

        mQuery.prepare( QStringLiteral( "SELECT DISTINCT headers.byteLength, " )
                        % QStringLiteral( "headers.seekIndex, headers.result, headers.infoSystemName FROM headers " )
                        % QStringLiteral( "INNER JOIN systems " )
                        % QStringLiteral( "WHERE systems.phoenixSystemName = ?" ) );

        qDebug() << system << canonicalFilePath();
        mQuery.addBindValue( system );

        auto exec = mQuery.exec();
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mQuery.lastError().text() ) );

        int count = 0;
        while( mQuery.next() ) {

            headerData.byteLength = mQuery.value( 0 ).toInt();
            headerData.seekPosition = mQuery.value( 1 ).toInt();
            headerData.result = mQuery.value( 2 ).toString();
            headerData.phoenixSystemName = system;

            count++;
        }
        Q_ASSERT_X( count == 1, Q_FUNC_INFO, "count != 1" );
    }

    return std::move( headerData );
}

QString GameFileInfo::getCheckSum( const QString &filePath ) {
    QString hash;
    QFile file( filePath );

    if( file.open( QIODevice::ReadOnly ) ) {

        auto crc32 = CryptoHash( CryptoHash::Crc32 );

        crc32.addData( &file );

        hash = crc32.result();

        file.close();
    }

    return std::move( hash );
}

void GameFileInfo::update( const QString &extension ) {
    auto possibleSystemsList = getAvailableSystems( extension );

    if ( possibleSystemsList.size() == 1 ) {
        mSystem = possibleSystemsList.at( 0 );
    } else {
        auto header = getPossibleHeaders( possibleSystemsList );
        mSystem = header.phoenixSystemName;
    }

    mFullFilePath = QStringLiteral( "file://" ) % canonicalFilePath();
    mCrc32Checksum = getCheckSum( canonicalFilePath() );
}

bool GameFileInfo::isBios( QString &biosName ) {

    static const QString statement = QStringLiteral( "SELECT biosFile FROM firmware WHERE sha1 = ?" );

    QFile file( canonicalFilePath() );

    auto opened = file.open( QIODevice::ReadOnly );
    Q_ASSERT( opened );

    QCryptographicHash sha1( QCryptographicHash::Sha1 );

    sha1.addData( &file );
    auto sha1Result = sha1.result().toHex().toUpper();

    file.close();

    mQuery.prepare( statement );
    mQuery.addBindValue( sha1Result );

    auto exec = mQuery.exec();
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mQuery.lastError().text() ) );

    if ( mQuery.first() ) {
        biosName = mQuery.value( 0 ).toString();

        return !biosName.isEmpty();
    }

    return false;
}

