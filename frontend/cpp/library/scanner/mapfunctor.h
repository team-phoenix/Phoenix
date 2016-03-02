#ifndef MAPFUNCTOR_H
#define MAPFUNCTOR_H

#include "librarytypes.h"
#include "scannerutil.h"

// Step 1: Build a list of file paths from a list of file and folder paths by enumerating folders

// Step 2: Expand list of file paths by enumerating archive files, caching hashes if available

// Step 3: Eliminate redundant .bin files that were listed in .cue files
// This is an optimization: .bin files tend to be huge and time-consuming to hash. If a .cue file that matches
// against the database lists a bunch of .bin files that have yet to be scanned, the .bin files can be skipped

// Step 4: Match file list against game database
// TODO: Change to hashing only

namespace Library {

    class MapFunctor
    {
    public:
        enum Step {

            // If a file path, returns a single-element list containing that file path
            // Otherwise, enumerate that directory and return list of all files within
            // TODO: Recursion?
            One,

            // Enumerate archive file if given one, caching hashes if the format supports the ones we need.
            // Only archive files will have more than one entry in the return list, and only archive file formats that
            // store checksums will have cached checksum entries in the list
            Two,

            // Enumerate .bin files in each .cue file, return the list of .bin files without checking if the .bin files exist
            // Prepend .bin file paths with directory .cue file resides in.
            Three,

            // Match against game database by hash, falling back in the following order if that misses:
            // - Filename matching, making sure the extension is valid for the system (fuzzy matching?)
            // - Header (signature/magic number) matching
            // - File extension matching
            // - Giving up (the user will have to manually match it against a system or ignore it)
            Four,
        };

        enum SearchReason {
            GetROMID,
            GetArtwork,
            GetSystemUUID,
            GetMetadata,
            GetHeaders,
        };

        MapFunctor( const Step step );

        bool searchDatabase( const SearchReason reason, FileEntry &fileEntry );

        // This alias is needed by QtConcurrent in order to use a
        // functor. The result of the operator() must have an alias
        // or a typedef with the name 'result_type'.

        using result_type = FileList;
        FileList operator()( const FileEntry &entry );
        FileList operator()( const QString &path );

    private:
        Step mStep;

        QString getLastExecutedQuery( const QSqlQuery &query );

    };

}

#endif // MAPFUNCTOR_H
