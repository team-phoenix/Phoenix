#include "gamescanner.h"

#include "cuefile.h"

GameScanner::GameScanner( QObject *parent ) : QObject( parent ) {

}

QStringList GameScanner::stepOneMap( const QString &path ) {
    QStringList filePathList;
    QFileInfo dir( path );

    qDebug() << path;

    if( dir.isFile() ) {
        filePathList.append( path );
        return filePathList;
    }

    // path is a file system directory past this point.
    QDir directory( path );

    if( !directory.exists() ) {
        return QStringList();
    }

    QDirIterator dirIter( path, QDir::Files, QDirIterator::NoIteratorFlags );

    while( dirIter.hasNext() ) {
        const QString filePath = dirIter.next();

        if( QFile::exists( filePath ) ) {
            filePathList.append( filePath );
        }
    }

    return filePathList;
}

void GameScanner::stepOneReduce( QStringList &mergedList, const QStringList &givenList ) {
    if( !givenList.isEmpty() ) {
        mergedList.append( givenList );
    }
}

GameScanner::FileList GameScanner::stepTwoMap( const QString &filePath ) {
    QStringList filePathList;

    QFileInfo info( filePath );

    if( info.suffix() == QStringLiteral( "zip" ) ) {
        // Expand and explore the zip file.
        qDebug() << "Found zip file:" << filePath;

        ArchiveFile::ParseData parsedData = ArchiveFile::parse( filePath );
        filePathList.append( parsedData );
    } else {
        filePathList.append( filePath );
    }

    return filePathList;
}

void GameScanner::stepTwoReduce( GameScanner::FileList &mergedList, const GameScanner::FileList &givenList ) {
    mergedList.append( giveList );
}

GameScanner::FileList GameScanner::stepThreeMap( const GameScanner::FileEntry &fileEntry )  {

    GameScanner::FileList filePathList;

    QFileInfo info( fileEntry.filePath );
    if( info.suffix() == QStringLiteral( "cue" ) ) {
        // Explore the cue file.
        qDebug() << "Found cue file: " << entry.filePath;
        QStringList binFiles = CueFile::parse( entry.filePath );
        for ( QString binFile : binFiles ) {
            GameScanner::FileEntry entry;
            entry.filePath = binFile;
            filePathList.append( entry );
        }
    }

    return filePathList;
}

void GameScanner::stepThreeReduce(GameScanner::FileList &mergedList, const GameScanner::FileList &givenList) {
    if ( !givenList.isEmpty() ) {
        mergedList.append( givenList );
    }
}

GameScanner::FileList GameScanner::stepFourFilter( const GameScanner::FileList &fileList ) {

    for ( FileEntry entry : fileList ) {
        QFileInfo info( entry.filePath );
        if ( info.suffix() == QStringLiteral( "bin" ) ) {
            // Delete entry from main list.
            int i = 0;
            for ( FileEntry mainEntry : mFileList ) {
                if ( mainEntry.filePath == entry.filePath ) {
                    mFileList.removeAt( i );
                }
                i++;
            }
        }
    }

}


