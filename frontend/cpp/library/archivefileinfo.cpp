#include "archivefileinfo.h"

using namespace Library;

ArchiveFileInfo::ArchiveFileInfo(const QString &file)
    : GameFileInfo( file ),
      mZipFile( nullptr ),
      mIsValid( true ) {

    mFileType = FileType::ZipFile;

}

ArchiveFileInfo::ArchiveFileInfo(GameFileInfo &gameInfo)
    : ArchiveFileInfo( gameInfo.canonicalFilePath() )
{

}

bool ArchiveFileInfo::firstFile()
{
    bool next = mZipFile->goToFirstFile();
    if ( next ) {
        update();
    }
    return next;
}

bool ArchiveFileInfo::nextFile() {
    bool next = mZipFile->goToNextFile();

    if ( next ) {
        update();
    }

    return next;
}

bool ArchiveFileInfo::isValid() const
{
    return mIsValid;
}

bool ArchiveFileInfo::open( QuaZip::Mode mode ) {
    if ( !mZipFile ) {
        mZipFile = new QuaZip( canonicalFilePath() );
    }
    return mZipFile->open( mode );
}

void ArchiveFileInfo::close()
{
    mZipFile->close();
    delete mZipFile;
}

QString ArchiveFileInfo::nextFileName() const
{
    return mZipFile->getCurrentFileName();
}

QString ArchiveFileInfo::delimiter()
{
    return QStringLiteral( "<><>" );
}

void ArchiveFileInfo::update() {

    QuaZipFileInfo zipFileInfo;
    if ( mZipFile->getCurrentFileInfo( &zipFileInfo ) ) {
        mCrc32Checksum = QString::number( zipFileInfo.crc, 16 ).toUpper();
    }

    auto fileInfo = QFileInfo( nextFileName() );

    if ( fileInfo.suffix() == QStringLiteral( "zip" ) ) {
        mIsValid = false;
        return;
    }

    auto possibleSystemsList = getAvailableSystems( fileInfo.suffix() );

    mIsValid = !possibleSystemsList.isEmpty();

    if ( possibleSystemsList.size() == 1 ) {
        mSystem = possibleSystemsList.at( 0 );
    }

    mTitle = nextFileName().left( nextFileName().lastIndexOf( '.' ) );
    mFullFilePath = QStringLiteral( "zip://" ) + canonicalFilePath()
            + ArchiveFileInfo::delimiter() + nextFileName();

}

