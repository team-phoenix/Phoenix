#ifndef GAMESCANNER_H
#define GAMESCANNER_H

#include "frontendcommon.h"

#include "archivefile.h"

// Phoenix's game scanner takes advantage of MapReduce (and Qt's easy-to-use implementation of it) to efficiently and
// asyncronously scan large numbers of files

class GameScanner : public QObject {
        Q_OBJECT

        // A struct that attaches some basic metadata to a file path
        typedef struct FileEntryStruct {
            FileEntryStruct() : filePath(), hash( 0 ), hasHashCached( false ), gameUUID(), systemUUIDs(), type( NOT_YET_SCANNED ) {}

            // Absolute path to a file
            QString filePath;

            // CRC32 hash for matching against game database
            quint32 hash; // TODO: Use a more specific type to store the hash?

            // Do we have a hash cached?
            bool hasHashCached;

            // Database UUID
            QString gameUUID;

            // Some extensions may map to multiple systems, keep a list in order to present this info to the user later
            QStringList systemUUIDs;

            // The result of the game scanner. Defines which of the above members have valid values
            gameScannerResult scannerResult;
        } FileEntry;

        // Extra data that holds the result of the game scanner
        typedef enum {
            // Default value, not yet scanned
            NOT_YET_SCANNED,

            // Hit against game database by hash or filename matching (TODO: separate?)
            // Implies that the system UUID is known too
            GAME_UUID_KNOWN,

            // Hit against system database by extension (only one system uses the extension)
            // systemUUIDs contains one element
            SYSTEM_UUID_KNOWN,

            // Hit against system database by extension (multiple systems use the extension)
            // systemUUIDs contains two or more elements
            MULTIPLE_SYSTEM_UUIDS,

            // Miss against game and system database
            SYSTEM_UUID_UNKNOWN,

            // Path is a .bin file that is listed in a valid .cue file. Paths marked with this value should not be scanned
            // in step 4.
            PART_OF_CUE_FILE
        } gameScannerResult;

        typedef QList<FileEntry> FileList;

    public:
        explicit GameScanner( QObject *parent = 0 );

        // Step 1: Build a list of file paths from a list of file and folder paths by enumerating folders

        // If a file path, returns a single-element list containing that file path
        // Otherwise, enumerate that directory and return list of all files within
        // TODO: Recursion?
        static QStringList stepOneMap( const QString &path );

        // Merge lists together into one main list
        static void stepOneReduce( QStringList &mergedList, const QStringList &givenList );

        // Step 2: Expand list of file paths by enumerating archive files, caching hashes if available

        // Enumerate archive file if given one, caching hashes if the format supports the ones we need.
        // Only archive files will have more than one entry in the return list, and only archive file formats that
        // store checksums will have cached checksum entries in the list
        static FileList stepTwoMap( const QString &filePath );

        // Merge lists together into one main list
        static void stepTwoReduce( FileList &mergedList, const FileList &givenList );

        // Step 3: Eliminate redundant .bin files that were listed in .cue files
        // This is an optimization: .bin files tend to be huge and time-consuming to hash. If a .cue file that matches
        // against the database lists a bunch of .bin files that have yet to be scanned, the .bin files can be skipped

        // Filter .cue files
        // This could be done by MapReduce too, but since it's just some list maniuplation we'll let that slide
        static FileList stepThreeFilter( const FileList &inputPathList );

        // Enumerate .bin files in each .cue file, return the list of .bin files without checking if the .bin files exist
        // Prepend .bin file paths with directory .cue file resides in
        static FileList stepThreeMap( const FileEntry &cueFilePath );

        // Merge lists together into one main list
        static void stepThreeReduce( FileList &mergedList, const FileList &givenList );

        // Step 4: Match file list against game database

        // Mark .bin files from main list that came from step 3's output as not needing to be scanned (cheaper than removing?)
        // FIXME: This operation is O( stepThreeOutputCount * mainListCount ) ~= O( n^2 )... bad enough it should be its own step?
        static FileList stepFourFilter( const FileList &fileList );

        // Match against game database by hash, falling back in the following order if that misses:
        // - Filename matching, making sure the extension is valid for the system (fuzzy matching?)
        // - Header (signature/magic number) matching
        // - File extension matching
        // - Giving up (the user will have to manually match it against a system or ignore it)
        static FileEntry stepFourMap( const FileEntry &filePath );

        // Merge entries together into one main list
        static void stepFourReduce( FileList &mergedList, const FileEntry &givenEntry );

    signals:

    public slots:
};

#endif // GAMESCANNER_H
