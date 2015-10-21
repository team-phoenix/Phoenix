#include "cuefileinfo.h"

using namespace Library;

CueFileInfo::CueFileInfo( const QString &file )
    : GameFileInfo( file )
{

}


CueFileInfo::CueFileInfo(GameFileInfo &gameInfo)
    : CueFileInfo( gameInfo.canonicalFilePath() )
{

}

bool CueFileInfo::isValid() {

    auto gameFiles = files();
    if ( gameFiles.isEmpty() ) {
        return false;
    }

    auto exists = QFile::exists( gameFiles.first() );

    auto possibleSystemsList = getAvailableSystems( suffix() );

    if ( possibleSystemsList.size() == 1 ) {
        mSystem = possibleSystemsList.at( 0 );
    } else {
        if ( exists ) {
            auto localFile = gameFiles.first();

            auto gameInfo = GameFileInfo( localFile );

            auto header = gameInfo.getPossibleHeaders( possibleSystemsList );
            mSystem = header.phoenixSystemName;
        }
    }

    mFullFilePath = QStringLiteral( "cue://" ) + canonicalFilePath();
    mCrc32Checksum = getCheckSum( canonicalFilePath() );

    return exists;
}

QStringList CueFileInfo::files() {

    QFile cueFile( canonicalFilePath() );
    QStringList gameFiles;

    if( !cueFile.open( QIODevice::ReadOnly ) ) {
        return gameFiles;
    }

    while( !cueFile.atEnd() ) {
        auto line = cueFile.readLine();

        QString obtainedTitle;
        bool eatChars = false;

        for( auto &subStr : line ) {
            if( subStr == '"' ) {
                if( eatChars ) {
                    eatChars = false;
                } else {
                    eatChars = true;

                    if( obtainedTitle.isEmpty() ) {
                        continue;
                    }
                }
            }

            if( eatChars ) {
                obtainedTitle.append( subStr );
            }
        }

        if( !obtainedTitle.isEmpty() ) {
            auto gameFile = canonicalPath() + QDir::separator() + obtainedTitle;
            gameFiles.append( gameFile );
        }

    }

    cueFile.close();

    return gameFiles;
}


