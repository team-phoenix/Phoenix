#include "biosfileinfo.h"

using namespace Library;

BiosFileInfo::BiosFileInfo( const QString &file )
    : GameFileInfo( file ) {

}


BiosFileInfo::BiosFileInfo( Library::GameFileInfo &gameInfo )
    : BiosFileInfo( gameInfo.canonicalFilePath() ) {

}

/*

*/
