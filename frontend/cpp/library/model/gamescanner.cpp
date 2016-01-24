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

    // Guard against ToC to ToU errors (attacks?)
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
    mergedList.append( givenList );
}

GameScanner::FileList GameScanner::stepTwoMap( const QString &filePath ) {
    FileList fileList;

    QFileInfo info( filePath );

    if( info.suffix() == QStringLiteral( "zip" ) ) {
        // Expand and explore the zip file.
        qDebug() << "Found zip file:" << filePath;

        ArchiveFile::FileToHashMap parsedData = ArchiveFile::parse( filePath );

        // Insert path and hash cache into main list for this zip for each file
        for( QString path : parsedData ) {
            FileEntry fileEntry;
            fileEntry.filePath = path;
            fileEntry.hash = parsedData[path];
            fileEntry.hasHashCached = true;
            fileList.append( fileEntry );
        }
    } else {
        // Insert a single entry into this list run's output list
        FileEntry fileEntry;
        fileEntry.filePath = filePath;
        fileList.append( fileEntry );
    }

    return fileList;
}

void GameScanner::stepTwoReduce( GameScanner::FileList &mergedList, const GameScanner::FileList &givenList ) {
    mergedList.append( givenList );
}

QStringList GameScanner::stepThreeMap( const GameScanner::FileEntry &fileEntry ) {
    QStringList binList;

    QFileInfo info( fileEntry.filePath );

    if( info.suffix() == QStringLiteral( "cue" ) ) {
        // Explore the cue file.
        qDebug() << "Found cue file: " << fileEntry.filePath;
        binList = CueFile::parse( fileEntry.filePath );
    }

    return binList;
}

void GameScanner::stepThreeReduce( QStringList &mergedList, const QStringList &givenList ) {
    mergedList.append( givenList );
}

GameScanner::FileList GameScanner::stepFourFilter( const QStringList &binList ) {
    for( FileEntry &fileEntry : mFileList ) {
        QFileInfo info( fileEntry.filePath );

        // Check the list's .bin files to see if it's a .bin file named in a .cue file and mark it as skippable
        if( info.suffix() == QStringLiteral( "bin" ) ) {
            // FIXME: Handle case insensitivity on filesystems used by Windows such as FAT32 or NTFS
            if( binList.contains( fileEntry.filePath /*, Qt::CaseInsensitive */ ) ) {
                fileEntry.scannerResult = PART_OF_CUE_FILE;
            }
        }
    }
}


