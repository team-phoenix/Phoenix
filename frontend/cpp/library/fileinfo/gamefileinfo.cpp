#include "gamefileinfo.h"
#include "cryptohash.h"

#include <QStringBuilder>

using namespace Library;

GameFileInfo::GameFileInfo( const QString &file )
    : QFileInfo( file ),
      mFileType( FileType::UnsupportedFile ),
      mSystem(),
      mPossibleSystemsList(),
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
        fillBasicInfo();
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
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() % " -- " % query.lastQuery() ) );

        while( query.next() ) {
            auto extension = query.value( 0 ).toString();
            filter.append( QStringLiteral( "*." ) + extension );
        }
    }

    return filter;
}

QStringList GameFileInfo::getSystemListForExtension( const QString &extension ) {
    mLibretroQuery.prepare( QStringLiteral( "SELECT DISTINCT system.UUID FROM system"
                                            " INNER JOIN extension ON system.UUID=extension.system"
                                            " WHERE extension.extension = :extension AND system.enabled=1" ) );

    mLibretroQuery.bindValue( ":extension", extension );

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

void GameFileInfo::scanOpenVGDBForGame() {

    QSqlQuery openVGDBQuery( MetaDataDatabase::database() );
    bool ret = false;

    openVGDBQuery.prepare( QStringLiteral( "SELECT romID FROM " OPENVGDBTABLEROMS " WHERE romHashCRC LIKE :mCrc32Checksum" ) );
    openVGDBQuery.bindValue( QStringLiteral( ":mCrc32Checksum" ), mCrc32Checksum );

    ret = openVGDBQuery.exec();
    Q_ASSERT_X( ret, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

    // If we found a match, ask OpenVGDB for the rest
    if( openVGDBQuery.first() ) {
        fillMetadataFromOpenVGDB( openVGDBQuery.value( 0 ).toInt(), openVGDBQuery );
    }

    // If that got us nothing, try searching by filename
    else {
        QFileInfo file( mFullFilePath );
        QString filename = file.fileName();

        // Filename must be sanitized before being passed into an SQL query
        openVGDBQuery.prepare( QStringLiteral( "SELECT romID FROM " OPENVGDBTABLEROMS " WHERE romFileName = :romFileName" ) );
        openVGDBQuery.bindValue( QStringLiteral( ":romFileName" ), filename );

        ret = openVGDBQuery.exec();
        Q_ASSERT_X( ret, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

        if( openVGDBQuery.first() ) {
            fillMetadataFromOpenVGDB( openVGDBQuery.value( 0 ).toInt(), openVGDBQuery );
        }
    }

}

void GameFileInfo::fillMetadataFromOpenVGDB( int romID, QSqlQuery &openVGDBQuery ) {

    if( romID > -1 ) {
        openVGDBQuery.prepare( QStringLiteral( "SELECT RELEASES.releaseCoverFront, SYSTEMS.systemName FROM ROMs "
                                               "INNER JOIN SYSTEMS ON SYSTEMS.systemID = ROMs.systemID "
                                               "INNER JOIN RELEASES ON RELEASES.romID = ROMs.romID "
                                               "WHERE ROMs.romID = :romID" ) );

        openVGDBQuery.bindValue( QStringLiteral( ":romID" ), romID );

        auto exec = openVGDBQuery.exec();
        Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( openVGDBQuery.lastError().text() % " -- " % getLastExecutedQuery( openVGDBQuery ) ) );

        if( openVGDBQuery.first() ) {

            mArtworkUrl = openVGDBQuery.value( 0 ).toString();

            // Get the Phoenix UUID
            mLibretroQuery.prepare( QStringLiteral( "SELECT UUID, enabled FROM system "
                                                    "WHERE openvgdbSystemName=:openvgdbSystemName" ) );
            mLibretroQuery.bindValue( ":openvgdbSystemName", openVGDBQuery.value( 1 ).toString() );

            auto exec = mLibretroQuery.exec();
            Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( mLibretroQuery.lastError().text() % " -- " % mLibretroQuery.lastQuery() ) );
            //qCDebug( phxLibrary ) << mLibretroQuery.lastQuery();

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

QString GameFileInfo::getLastExecutedQuery( const QSqlQuery &query ) {
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

GameFileInfo::HeaderData GameFileInfo::getPossibleHeaders( const QStringList &possibleSystems ) {

    HeaderData headerData;

    for( auto &system : possibleSystems ) {

        QString statement = QStringLiteral( "SELECT DISTINCT header.byteLength, header.seekIndex, header.result,"
                                            " header.system FROM header INNER JOIN system ON system.UUID=\'%1\'"
                                            " WHERE system.UUID=header.system AND system.enabled=1" );
        statement = statement.arg( system );

        auto exec = mLibretroQuery.exec( statement );
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

    return std::move( headerData );
}

QString GameFileInfo::getCRC32AsQString( const QString &filePath ) {
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

void GameFileInfo::fillBasicInfo() {
    const QString extension = suffix();
    mPossibleSystemsList = getSystemListForExtension( extension );

    // No match for this extension means mSystem will be empty, ensuring the game will not be imported unless there's
    // a match by hash from the DB
    if( mPossibleSystemsList.size() == 0 ) {
        return;
    }

    // Implicitly set the system for this file here. Will not be overwritten if the scanner fails to match
    mSystem = mPossibleSystemsList.at( 0 );

    if( mPossibleSystemsList.size() == 1 ) {
        mSystem = mPossibleSystemsList.at( 0 );
    } else {
        auto header = getPossibleHeaders( mPossibleSystemsList );
        mSystem = header.system;
    }

    mFullFilePath = QStringLiteral( "file://" ) % canonicalFilePath();
    mCrc32Checksum = getCRC32AsQString( canonicalFilePath() );
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

