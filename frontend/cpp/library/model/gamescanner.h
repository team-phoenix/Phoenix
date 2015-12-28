#ifndef GAMESCANNER_H
#define GAMESCANNER_H

#include "frontendcommon.h"

// A set of MapReduce functions for GameScanController. These functions exist in a separate class to keep the code cleaner.
// Note that static linking is necessary unless the functions were to return objects of type GameScanner, which they
// never will.

class GameScanner : public QObject {
        Q_OBJECT

        // Step-specific structs and enums. These hold some extra data along with the usual file path

        typedef struct stepTwoOutputEntryStruct {
            stepTwoOutputEntryStruct() : filePath(), hash( 0 ), hasHash( false ) {}
            QString filePath;
            quint32 hash; // TODO: proper type for hash exists?
            bool hasHash;
        } stepTwoOutputEntry;

        typedef QList<stepTwoOutputEntry> stepTwoOutput;

        typedef stepTwoOutput stepThreeInput;

        // Extra data that holds the result of the game scanner
        typedef enum {
            // Default value, not yet scanned
            NOT_YET_SCANNED,

            // Hit against game database by hash or filename matching (TODO: separate?)
            // Implies that the system UUID is known too
            GAME_UUID_KNOWN,

            // Hit against system database by extension (only one system uses the extension)
            SYSTEM_UUID_KNOWN,

            // Hit against system database by extension (multiple systems use the extension)
            MULTIPLE_SYSTEM_UUIDS,

            // Miss against game and system database
            SYSTEM_UUID_UNKNOWN,

            // Path is a .bin file that is listed in a valid .cue file. This path is marked to be pruned between steps
            // 3 and 4. It doesn't matter if the .cue itself matches against the DB.
            // This is an optimization: .bin files tend to be huge and expensive to hash. If the .cue file associated with
            // can be matched against the database, the .bin files can be skipped, saving a lot of scanning time.
            PART_OF_CUE_FILE
        } stepThreeOutputType;

        typedef struct stepThreeOutputEntryStruct {
            stepThreeOutputEntryStruct(): filePath(), gameUUID(), systemUUIDs(), type( NOT_YET_SCANNED ) {}
            QString filePath;
            QString gameUUID;
            QStringList systemUUIDs;
            stepThreeOutputType type;
        } stepThreeOutputEntry;

        typedef QList<stepThreeOutputEntry> stepThreeOutput;

    public:
        explicit GameScanner( QObject *parent = 0 );

        // Step 1: Build a list of file paths from a list of file and folder paths by enumerating folders

        // If a file path, returns a single-element list containing that file path
        // Otherwise, enumerate that directory and return list of all files within
        static QStringList stepOneMap( const QString &path );

        // Merge lists together into one main list
        static void stepOneReduce( QStringList &outputPathList, const QStringList &inputPathList );

        // Step 2: Expand list of file paths by enumerating archive files, caching hashes if available

        // Enumerate archive file if given one, caching hashes if the format supports the ones we need.
        // Only archive files will have multiple entries in the return value, and only archive files formats that give
        // us checksums will have a cache
        static stepTwoOutput stepTwoMap( const QString &filePath );

        // Merge lists together into one main list
        static void stepTwoReduce( stepTwoOutput &outputPathList, const stepTwoOutput &inputPathList );

        // Step 3: Match .cue files first against DB

        // Filter out .cue files
        // This could be done by MapReduce too, but since it's just some list maniuplation we'll let that slide
        static stepThreeInput stepThreeFilter( const stepTwoOutput &inputPathList );

        //

    signals:

    public slots:
};

#endif // GAMESCANNER_H
