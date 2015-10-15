#include "archievefileinfo.h"
#include "systemdatabase.h"
#include "metadatadatabase.h"
#include "quazip.h"

#include <QSqlQuery>

using namespace Library;

ArchieveFileInfo::ArchieveFileInfo(const QString &file)
    : GameFileInfo( file ),
      mZipFile( nullptr ),
      mIsValid( true ) {

    mFileType = FileType::ZipFile;

}

ArchieveFileInfo::ArchieveFileInfo(GameFileInfo &gameInfo)
    : ArchieveFileInfo( gameInfo.canonicalFilePath() )
{

}

bool ArchieveFileInfo::firstFile()
{
    bool next = mZipFile->goToFirstFile();
    if ( next ) {
        update();
    }
    return next;
}

bool ArchieveFileInfo::nextFile() {
    bool next = mZipFile->goToNextFile();

    if ( next ) {
        update();
    }

    return next;
}

bool ArchieveFileInfo::isValid() const
{
    return mIsValid;
}

bool ArchieveFileInfo::open( QuaZip::Mode mode ) {
    if ( !mZipFile ) {
        mZipFile = new QuaZip( canonicalFilePath() );
    }
    return mZipFile->open( mode );
}

void ArchieveFileInfo::close()
{
    mZipFile->close();
    delete mZipFile;
}

QString ArchieveFileInfo::nextFileName() const
{
    return mZipFile->getCurrentFileName();
}

QString ArchieveFileInfo::delimiter()
{
    return QStringLiteral( "<><>" );
}

void ArchieveFileInfo::update() {

    QuaZipFileInfo zipFileInfo;
    if ( mZipFile->getCurrentFileInfo( &zipFileInfo ) ) {
        mCrc32Checksum = QString::number( zipFileInfo.crc, 16 ).toUpper();
    }

    auto fileInfo = QFileInfo( nextFileName() );

    auto possibleSystemsList = getAvailableSystems( fileInfo.suffix() );

    //if ( possibleSystemsList.isEmpty() )
    mIsValid = !possibleSystemsList.isEmpty();

    if ( possibleSystemsList.size() == 1 ) {
        mSystem = possibleSystemsList.at( 0 );
    }

    mTitle = nextFileName().left( nextFileName().lastIndexOf( '.' ) );
    mFullFilePath = QStringLiteral( "zip://" ) + canonicalFilePath()
            + ArchieveFileInfo::delimiter() + nextFileName();

}

