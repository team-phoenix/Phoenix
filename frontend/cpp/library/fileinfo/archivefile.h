#ifndef ARCHIVEFILE_H
#define ARCHIVEFILE_H

#include "quazipfile.h"
#include "quazip.h"

class ArchiveFile
{
public:
    ArchiveFile( const QString &file );

    typedef QHash<QString, quint32> FileToHashMap;

    static FileToHashMap parse( const QString &file );

private:
    static const QString delimiter();
    static const QString prefix();

};

Q_DECLARE_METATYPE( ArchiveFile::ParseData )

#endif // ARCHIVEFILE_H
