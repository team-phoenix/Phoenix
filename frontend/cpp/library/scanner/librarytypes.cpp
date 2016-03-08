#include "librarytypes.h"

using namespace Library;

FileEntry::FileEntry()
    : hasHashCached( false ),
      scannerResult( NotYetScanned ) {

}

FileEntry::FileEntry( const FileEntry &other ) {
    filePath = other.filePath;
    crc32 = other.crc32;
    hasHashCached = other.hasHashCached;
    gameUUID = other.gameUUID;
    systemUUIDs = other.systemUUIDs;
    scannerResult = other.scannerResult;
    gameMetadata = other.gameMetadata;
}

QDebug Library::operator<<( QDebug debug, const FileEntry &entry ) {

    debug.nospace() << qPrintable( QString(
                                       "\n\tfilePath = %1\n"
                                       "\tcrc32 = %2\n"
                                       "\thasHashCached = %3\n"
                                       "\tgameUUID = %4\n"
                                   ).
                                   arg( entry.filePath )
                                   .arg( entry.crc32 )
                                   .arg( entry.hasHashCached )
                                   .arg( entry.gameUUID )
                                 );

    debug.nospace() << "\tSystem UUIDs: " << entry.systemUUIDs << "\n";
    debug.nospace() << "\tScanner result: " << entry.scannerResult << "\n";
    debug.nospace() << "\tMetadata:\n" << entry.gameMetadata;
    return debug.maybeSpace();
}

QDebug Library::operator<<( QDebug debug, const GameMetaData &entry ) {

    debug.nospace() << qPrintable( QString(
                                       "\t\topenVGDBsystemName = %1\n"
                                       "\t\tfrontArtwork = %2\n"
                                       "\t\tgoodToolsCode = %3\n"
                                       "\t\tregion = %4\n"
                                       "\t\tdeveloper = %5\n"
                                       "\t\treleaseDate = %6\n"
                                       "\t\tgenre = %7\n"
                                       "\t\tdescription = %8\n"
                                       "\t\ttitle = %9\n"
                                   ).
                                   arg( entry.openVGDBsystemName )
                                   .arg( entry.frontArtwork )
                                   .arg( entry.goodToolsCode )
                                   .arg( entry.region )
                                   .arg( entry.developer )
                                   .arg( entry.releaseDate )
                                   .arg( entry.genre )
                                   .arg( entry.description )
                                   .arg( entry.title )
                                 );
    debug.nospace() << qPrintable( QString(
                                       "\t\tidentifier = %1\n"
                                       "\t\tprogress = %2\n"
                                       "\t\tupdated = %3\n"
                                       "\t\tromID = %4\n"
                                   ).
                                   arg( entry.identifier )
                                   .arg( entry.progress )
                                   .arg( entry.updated )
                                   .arg( entry.romID )
                                 );
    return debug.maybeSpace();
}
