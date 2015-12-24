#ifndef ARCHIVEFILE_H
#define ARCHIVEFILE_H

#include "quazipfile.h"
#include "quazip.h"

class ArchiveFile
{
public:
    ArchiveFile( const QString &file );

    struct ParseData {
        QStringList enumeratedFiles;
        QHash<QString, QString> fileHashesMap;
    };

    static ParseData parse( const QString &file );

private:
    static const QString delimiter();
    static const QString prefix();

};

Q_DECLARE_METATYPE( ArchiveFile::ParseData )

#endif // ARCHIVEFILE_H
