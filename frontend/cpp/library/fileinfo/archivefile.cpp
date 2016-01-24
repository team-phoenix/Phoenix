#include "archivefile.h"

#include <QtCore>
#include <QStringBuilder>

ArchiveFile::ArchiveFile( const QString &file ) {

}

const QString ArchiveFile::delimiter() {
    return QStringLiteral( "|||" );
}

const QString ArchiveFile::prefix() {
    return QStringLiteral( "zip://" );
}

ArchiveFile::FileToHashMap ArchiveFile::parse( const QString &file ) {
    QuaZip zip( file );
    zip.open( QuaZip::mdUnzip );

    Q_ASSERT( zip.isOpen() );

    FileToHashMap fileHashMap;

    // Iterate through all files in the .zip
    for( bool f = zip.goToFirstFile(); f; f = zip.goToNextFile() ) {

        // Construct a path that we can detect later so it'll get handled differently from non .zip files
        const QString absPath = ArchiveFile::prefix() % file
                                % ArchiveFile::delimiter() % zip.getCurrentFileName();

        // Grab CRC32 for the current file
        QuaZipFileInfo zipFileInfo;

        if( zip.getCurrentFileInfo( &zipFileInfo ) ) {
            fileHashMap.insert( absPath, zipFileInfo.crc );
        }
    }

    return fileHashMap;
}

