#include "archievefileinfo.h"
#include "systemdatabase.h"
#include "quazip.h"

#include <QSqlQuery>

using namespace Library;

ArchieveFileInfo::ArchieveFileInfo(const QString &file)
    : GameFileInfo( file ),
      mZipFile( new QuaZip( file ) ),
      mIsValid( true ) {

    mFileType = FileType::ZipFile;

    if ( !mZipFile->open( QuaZip::mdUnzip ) ) {
        qCWarning( phxLibrary ) << "Couldn't open " << canonicalFilePath()
                                << ", errors: " << mZipFile->getZipError();
    }

}

ArchieveFileInfo::ArchieveFileInfo(GameFileInfo &gameInfo)
    : ArchieveFileInfo( gameInfo.canonicalFilePath() )
{

}

ArchieveFileInfo::~ArchieveFileInfo() {
    mZipFile->close();
    delete mZipFile;
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

QString ArchieveFileInfo::nextFileName() const
{
    return mZipFile->getCurrentFileName();
}

QString ArchieveFileInfo::delimiter()
{
    return QStringLiteral( "<><>" );
}

void ArchieveFileInfo::update() {
    GameFileInfo::update( QFileInfo( nextFileName() ).suffix() );

    mIsValid = !mSystem.isEmpty() && !nextFileName().isEmpty();
    mTitle = nextFileName().left( nextFileName().lastIndexOf( '.' ) );
    mFullFilePath = QStringLiteral( "zip://" ) + canonicalFilePath()
            + ArchieveFileInfo::delimiter() + nextFileName();
}
