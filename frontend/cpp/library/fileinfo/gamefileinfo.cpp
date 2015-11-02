#include "gamefileinfo.h"
#include "cryptohash.h"

#include <QStringBuilder>

using namespace Library;

GameFileInfo::GameFileInfo( const QString &file )
    : QFileInfo( file ),
      mFileType( FileType::UnsupportedFile ),
      mTitle( canonicalFilePath().remove( canonicalPath() ).remove( 0, 1 ).remove( QStringLiteral( "." ) + suffix() ) ),
      mFullFilePath( canonicalFilePath() ),
      mLibretroQuery( QSqlQuery( LibretroDatabase::database() ) ),
      mTimePlayed( QStringLiteral( "00:00" ) ) {

    if( QStringLiteral( "zip" ) == suffix() ) {
        mFileType = FileType::ZipFile;
    } else if( QStringLiteral( "cue" ) == suffix() ) {
        mFileType = FileType::CueFile;
    } else if( QStringLiteral( "bin" ) == suffix() ) {

        QString biosName;

        if( isBios( biosName ) ) {
            mFileType = FileType::BiosFile;
            mTitle = biosName;
        }

    } else if( QStringLiteral( "7z" ) == suffix() ) {

    } else if( QStringLiteral( "iso" ) == suffix() ) {

    } else {
        mFileType = FileType::GameFile;
        update( suffix() );
    }
}

QString GameFileInfo::system() const {
    return mSystem;
}

QString GameFileInfo::crc32CheckSum() const {
    return mCrc32Checksum;
}

QString GameFileInfo::title() const {
    return mTitle;
}

QString GameFileInfo::fullFilePath() const {
    return mFullFilePath;
}

QString GameFileInfo::timePlayed() const {
    return mTimePlayed;
}

QString GameFileInfo::crc32Checksum() const {
    return mCrc32Checksum;
}

QString GameFileInfo::artworkUrl() const {
    return mArtworkUrl;
}

GameFileInfo::FileType GameFileInfo::fileType() const {
    return mFileType;
}

QStringList GameFileInfo::gameFilter() {
    static QStringList filter;

    if( filter.isEmpty() ) {
        auto query = QSqlQuery( LibretroDatabase::database() );

        auto exec = query.exec( QStringLiteral( "SELECT DISTINCT extension FROM extension" ) );
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

        while( query.next() ) {
            auto extension = query.value( 0 ).toString();
            filter.append( QStringLiteral( "*." ) + extension );
        }
    }

    return filter;
}

QStringList GameFileInfo::getAvailableSystems( const QString &extension ) {
    mLibretroQuery.prepare( QStringLiteral( "SELECT DISTINCT system.UUID FROM system"
                                            " INNER JOIN extension ON system.UUID=extension.system"
                                            " WHERE extension.extension = ? AND system.enabled=1" ) );

    mLibretroQuery.addBindValue( extension );

    QStringList systemsList;


    auto exec = mLibretroQuery.exec();
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() ) );

    while( mLibretroQuery.next() ) {
        systemsList.append( mLibretroQuery.value( 0 ).toString() );
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

    // Get all of the romIDs to look up in the RELEASES table.
    if( query.first() ) {
        fillMetadata( query.value( 0 ).toInt(), query );
    }

    // If that got us nothing, try searching by filename
    else {
        QFileInfo file( mFullFilePath );
        QString filename = file.fileName();

        auto exec = query.exec( QString( "SELECT romID FROM %1 WHERE romFileName = \'%2\'" ).arg( MetaDataDatabase::tableRoms, filename ) );
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

        if( query.first() ) {
            fillMetadata( query.value( 0 ).toInt(), query );
        }
    }

}

void GameFileInfo::fillMetadata( int romID, QSqlQuery &metadataQuery ) {

    if( romID > -1 ) {

        static const auto statement = QStringLiteral( "SELECT RELEASES.releaseCoverFront, SYSTEMS.systemName FROM ROMs INNER JOIN SYSTEMS ON SYSTEMS.systemID = ROMs.systemID INNER JOIN RELEASES ON RELEASES.romID = ROMs.romID WHERE ROMs.romID = ?" );

        metadataQuery.prepare( statement );
        metadataQuery.addBindValue( romID );

        auto exec = metadataQuery.exec();
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( metadataQuery.lastError().text() ) );

        if( metadataQuery.first() ) {

            mArtworkUrl = metadataQuery.value( 0 ).toString();

            // Get the Phoenix UUID
            auto exec = mLibretroQuery.exec( QString( "SELECT UUID, enabled FROM system WHERE openvgdbSystemName=\'%1\'" ).arg( metadataQuery.value( 1 ).toString() ) );
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() ) );
            qCDebug( phxLibrary ) << mLibretroQuery.lastQuery();

            if( mLibretroQuery.first() ) {

                // Do not import games for systems that are disabled
                // Leaving system name blank will make game scanner skip it
                mSystem = "";
                if( mLibretroQuery.value( 1 ).toInt() ) {
                    mSystem = mLibretroQuery.value( 0 ).toString();
                }
            }


        }
    }
}

GameFileInfo::HeaderData GameFileInfo::getPossibleHeaders( const QStringList &possibleSystems ) {

    HeaderData headerData;

    for( auto &system : possibleSystems ) {

        QString statement = QStringLiteral( "SELECT DISTINCT header.byteLength, header.seekIndex, header.result,"
                                            " header.system FROM header INNER JOIN system ON system.UUID=\'%1\'"
                                            " WHERE system.UUID=header.system AND system.enabled=1" );
        statement = statement.arg( system );

        auto exec = mLibretroQuery.exec( statement );
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() ) );

        if ( mLibretroQuery.first() ) {

            headerData.byteLength = mLibretroQuery.value( 0 ).toInt();
            headerData.seekPosition = mLibretroQuery.value( 1 ).toInt();
            headerData.result = mLibretroQuery.value( 2 ).toString();
            headerData.system = system;

        } else {
            qCDebug( phxLibrary ) << "\n" << "Statement: " << statement << system << "\n" << mLibretroQuery.result();
        }

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

    if( possibleSystemsList.size() == 0 ) {
        return;
    }

    mSystem = possibleSystemsList.at( 0 );
    if( possibleSystemsList.size() == 1 ) {
        mSystem = possibleSystemsList.at( 0 );
    } else {
        auto header = getPossibleHeaders( possibleSystemsList );
        mSystem = header.system;
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

    mLibretroQuery.prepare( statement );
    mLibretroQuery.addBindValue( sha1Result );

    auto exec = mLibretroQuery.exec();
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() ) );

    if( mLibretroQuery.first() ) {
        biosName = mLibretroQuery.value( 0 ).toString();

        return !biosName.isEmpty();
    }

    return false;
}

