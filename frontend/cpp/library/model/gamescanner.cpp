#include "gamescanner.h"

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

