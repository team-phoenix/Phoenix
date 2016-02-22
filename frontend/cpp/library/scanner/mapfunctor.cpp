#include "mapfunctor.h"
#include "archivefile.h"
#include "cuefile.h"
#include "cryptohash.h"

#include <QFileInfo>
#include <QDirIterator>

using namespace Library;

MapFunctor::MapFunctor( const Step step )
    : mStep( step )
{

}

FileList MapFunctor::operator()(const FileEntry &entry) {
    FileList resultList;

    switch( mStep ) {
        case Two: {

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

            break;
        }
        case Three: {

            QFileInfo info( entry.filePath );

            if( info.suffix() == QStringLiteral( "cue" ) ) {
                // Explore the cue file.
                qDebug() << "Found cue file: " << entry.filePath;
                QStringList binFiles = CueFile::parse( entry.filePath );

                for( QString binFile : binFiles ) {
                    FileEntry newEntry;
                    newEntry.filePath = binFile;
                    resultList.append( newEntry );
                }
            } else {
                resultList.append( entry );
            }

            break;
        }
        case Four: {
            // Just go out and fill me all the data in at once.
            // If this returns false, then that means one of the ops failed.
            // I'm not sure which one, and at this stage I don't care.

            FileEntry entryCopy = entry;

            // Hash file
            CryptoHash crc32Hash( CryptoHash::Crc32 );

            if( crc32Hash.addData( entryCopy.filePath ) ) {
                entryCopy.hasHashCached = true;
                entryCopy.crc32 = crc32Hash.result();
            }

            resultList.append( entryCopy );

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

    qDebug() << "path: " << path;

    if( dir.isFile() ) {
        FileEntry entry;
        entry.filePath = path;
        resultList.append( entry );
    } else {

        // path is a file system directory past this point.
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

