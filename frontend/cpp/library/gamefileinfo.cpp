#include "gamefileinfo.h"
#include "JlCompress.h"
#include "systemdatabase.h"

#include <QtCore>

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

GameFileInfo::FileType GameFileInfo::fileType() const
{
    return mFileType;
}

QStringList GameFileInfo::gameFilter() {
    static QStringList filter;

    if ( filter.isEmpty() ) {
        auto query = QSqlQuery( SystemDatabase::database() );
        Q_ASSERT( query.exec( QStringLiteral( "SELECT DISTINCT extension FROM extensions" ) ) );

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

    qDebug() << canonicalFilePath() << mTitle;

    Q_ASSERT( gameFile.open( QIODevice::ReadOnly ) );

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

            Q_ASSERT( mQuery.exec() );

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

QList<GameFileInfo::HeaderData> GameFileInfo::getPossibleHeaders(const QStringList &possibleSystems) {

    QList<HeaderData> headerDataList;

    for( auto &system : possibleSystems ) {

        mQuery.clear();
        mQuery.prepare( QStringLiteral( "SELECT DISTINCT systemHeaderOffsets.byteLength, " )
                                     + QStringLiteral( "systemHeaderOffsets.seekIndex, systemHeaderOffsets.result, systemHeaderOffsets.systemIndex " )
                                     + QStringLiteral( "FROM systemHeaderOffsets INNER JOIN  systemMap ON " )
                                     + QStringLiteral( "systemMap.systemIndex=systemHeaderOffsets.systemIndex " )
                                     + QStringLiteral( "WHERE systemMap.systemname = ?" ) );
        mQuery.addBindValue( system );

        Q_ASSERT( mQuery.exec() );

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

    Q_ASSERT( mQuery.exec() );

    if ( mQuery.first() ) {
        biosName = mQuery.value( 0 ).toString();

        return !biosName.isEmpty();
    }

    return false;
}

