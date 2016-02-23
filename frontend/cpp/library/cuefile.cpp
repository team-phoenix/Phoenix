#include "cuefile.h"

#include <QtCore>
#include <QStringBuilder>

CueFile::CueFile()
{

}

QStringList CueFile::parse( const QString &filePath ) {

    // TODO: zip detection

    QFile cueFile( filePath );
    QFileInfo info( filePath );
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
            QDir dir = info.absoluteDir();
            gameFiles.append( dir.path() % obtainedTitle );
        }

    }

    return gameFiles;
}

const QString CueFile::delimiter()
{
    return QStringLiteral( "|||" );
}

const QString CueFile::prefix()
{
    return QStringLiteral( "cue://" );
}
