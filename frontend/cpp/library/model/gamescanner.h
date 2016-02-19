#ifndef GAMESCANNER_H
#define GAMESCANNER_H

#include "frontendcommon.h"
#include "archivefile.h"
#include "betterfuturewatcher.h"

#include <QObject>
#include <QFutureWatcher>

namespace Library {

    // Phoenix's game scanner takes advantage of MapReduce (and Qt's easy-to-use implementation of it) to efficiently and
    // asyncronously scan large numbers of files

    // Extra data that holds the result of the game scanner
    enum GameScannerResult {
        // Default value, not yet scanned
        NotYetScanned,

        // Hit against game database by hash or filename matching (TODO: separate?)
        // Implies that the system UUID is known too
        GameUUIDKnown,

        // Hit against system database by extension (only one system uses the extension)
        // systemUUIDs contains one element
        SystemUUIDKnown,

        // Hit against system database by extension (multiple systems use the extension)
        // systemUUIDs contains two or more elements
        MultipleSystemUUIDs,

        // Miss against game and system database
        SystemUUIDUnknown,

        // Path is a .bin file that is listed in a valid .cue file. Paths marked with this value should not be scanned
        // in step 4.
        PartOfCueFile,
    };

    // A struct that attaches some basic metadata to a file path
    struct FileEntry {
        FileEntry()
            : hasHashCached( false ),
              romID( -1 ),
              scannerResult( NotYetScanned )
        {

        }

        // Absolute path to a file
        QString filePath;

        // CRC32 hash for matching against game database
        QString crc32; // TODO: Use a more specific type to store the hash?

        // Do we have a hash cached?
        bool hasHashCached;

        // system name retrieved from the OpenVBDB database during a metadata search.
        QString openVGDBsystemName;

        // The romID value that is used as a UUID for the OpenVGDB for games.
        int romID;

        // Database UUID
        QString gameUUID;

        // Displayed artwork url for frontend.
        QString frontArtwork;

        // Some extensions may map to multiple systems, keep a list in order to present this info to the user later
        QStringList systemUUIDs;

        // The result of the game scanner. Defines which of the above members have valid values
        GameScannerResult scannerResult;
    };

    using FileList = QList<FileEntry>;

    class BetterFutureWatcher : public QObject {
        Q_OBJECT
    public:
        BetterFutureWatcher( QObject *parent = 0 )
            : QObject( parent ),
              mListIndex( -1 )
        {
            connect( &mWatcher, SIGNAL( finished() ), this, SLOT(slotInterceptFinished() ) );
        }

        void setFuture( const QFuture<FileList> &future, int index ) {
            mWatcher.setFuture( future );
            mListIndex = index;
        }

        QFutureWatcher<FileList> &futureWatcher() {
            return mWatcher;
        }

        int listIndex() const
        {
            return mListIndex;
        }

    signals:
        void finished( BetterFutureWatcher *watcher  );

    public slots:
        void slotInterceptFinished() {
            emit finished( this );
        }


    private:
        QFutureWatcher<FileList> mWatcher;
        int mListIndex;

    };

    class GameScanner : public QObject {
            Q_OBJECT

        public:
            explicit GameScanner( QObject *parent = 0 );


            enum SearchReason {
                GetROMID,
                GetArtwork,
                GetSystemUUID,
                GetMetadata,
                GetHeaders,
            };

            // Searches the OpenVGDB or the LibretroDB, in hopes of finding metadata.
            // The search reasons are listed in the enum. Most of these are used
            // internally. The main search reason to call is GetMetadata.
            static bool searchDatabase( const SearchReason reason, FileEntry &fileEntry );

            // Step 1: Build a list of file paths from a list of file and folder paths by enumerating folders

            // If a file path, returns a single-element list containing that file path
            // Otherwise, enumerate that directory and return list of all files within
            // TODO: Recursion?
            static FileList stepOneMap( const QString &pathList );

            // Merge lists together into one main list
            static void stepOneReduce( FileList &mergedList, const FileList &givenList );

            // Step 2: Expand list of file paths by enumerating archive files, caching hashes if available

            // Enumerate archive file if given one, caching hashes if the format supports the ones we need.
            // Only archive files will have more than one entry in the return list, and only archive file formats that
            // store checksums will have cached checksum entries in the list
            static FileList stepTwoMap( const FileEntry &filePath );

            // Merge lists together into one main list
            static void stepTwoReduce( FileList &mergedList, const FileList &givenList );

            // Step 3: Eliminate redundant .bin files that were listed in .cue files
            // This is an optimization: .bin files tend to be huge and time-consuming to hash. If a .cue file that matches
            // against the database lists a bunch of .bin files that have yet to be scanned, the .bin files can be skipped

            // Enumerate .bin files in each .cue file, return the list of .bin files without checking if the .bin files exist
            // Prepend .bin file paths with directory .cue file resides in
            static FileList stepThreeMap( const FileEntry &filePath );

            // Merge lists together into one main list
            static void stepThreeReduce( FileList &mergedList, const FileList &givenList );

            // Step 4: Match file list against game database

            // Mark .bin files from main list that came from step 3's output as not needing to be scanned (cheaper than removing?)
            // FIXME: This operation is O( stepThreeOutputCount * mainListCount ) ~= O( n^2 )... bad enough it should be its own step?
            static bool stepFourFilter( const FileEntry &fileEntry );

            // Merges entries recieved from filter into a single result.
            static void stepFourFilterReduce( FileList &mergedList, const FileEntry &givenList );

            // Match against game database by hash, falling back in the following order if that misses:
            // - Filename matching, making sure the extension is valid for the system (fuzzy matching?)
            // - Header (signature/magic number) matching
            // - File extension matching
            // - Giving up (the user will have to manually match it against a system or ignore it)
            static FileList stepFourMap( const FileEntry &filePath );

            // Merge entries together into one main list
            static void stepFourReduce( FileList &mergedList, const FileList &givenEntry );

        signals:

        public slots:

            void slotEnumeratePath( QString path );
            void stepOneFinished( BetterFutureWatcher *betterWatcher);
            void stepTwoFinished( BetterFutureWatcher *betterWatcher );
            void stepThreeFinished( BetterFutureWatcher *betterWatcher );
            void stepFourFilterFinished( BetterFutureWatcher *betterWatcher );
            void stepFourMapReduceFinished( BetterFutureWatcher *betterWatcher );

        private:

            // Contains a list of file paths that have been obtained by scanning the file system.

            FileList mFileList;
            QList<BetterFutureWatcher *> mWatcherList;

            // Helpers
                static QString getLastExecutedQuery( const QSqlQuery &query );

                // Checks if file is a bios file, if it is the QString passed to this
                // function will be updated with the proper bios name.
                static bool isBios( QFileInfo &info, QString &biosName );

    };

}

#endif // GAMESCANNER_H
