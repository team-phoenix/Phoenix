#include "biosfileinfo.h"
#include "systemdatabase.h"

#include <QtCore>

using namespace Library;

BiosFileInfo::BiosFileInfo( const QString &file )
    : GameFileInfo( file )
{

}


BiosFileInfo::BiosFileInfo(Library::GameFileInfo &gameInfo)
    : BiosFileInfo( gameInfo.canonicalFilePath() ) {

}

/*

*/
