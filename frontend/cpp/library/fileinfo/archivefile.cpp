#include "archivefile.h"

ArchiveFile::ArchiveFile( const QString &file ) {
    Q_UNUSED( file )
}

const QString ArchiveFile::delimiter() {
    return QStringLiteral( "|||" );
}

const QString ArchiveFile::prefix() {
    return QStringLiteral( "zip://" );
}

ArchiveFile::ParseData ArchiveFile::parse( const QString &file ) {
    KZip zip( file );
    zip.open( QIODevice::ReadOnly );

    // FIXME: Handle bad zip files instead of just crashing!
    Q_ASSERT( zip.isOpen() );

    QStringList zipFileList;
    QHash<QString, QString> fileHashMap;

    // TODO: Recurse
    // TODO: Support other file types
    for( const QString path : zip.directory()->entries() ) {
        auto *entry = dynamic_cast<const KZipFileEntry *>( zip.directory()->entry( path ) );

        // Skip folders (the contents of the folders should still be recursed)
        // Apparently this is guarantied by the standard?
        if( entry->isDirectory() ) {
            continue;
        }

        const QString absPath = ArchiveFile::prefix() % file
                                % ArchiveFile::delimiter() % entry->path();
        zipFileList.append( absPath );

        // Record the CRC32 for this entry in our model
        fileHashMap.insert( absPath, QString::number( entry->crc32(), 16 ).toUpper() );
    }

    ParseData data;
    data.enumeratedFiles = zipFileList;
    data.fileHashesMap = fileHashMap;

    return data;
}

