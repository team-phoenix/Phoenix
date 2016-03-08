#pragma once

#include "frontendcommon.h"

namespace Library {
    // GameMetaData is used to set in metadata for any game during;
    // this usually used after the append process.
    struct GameMetaData {
        GameMetaData()
            : progress( 0.0 ),
              updated( false ),
              romID( -1 ) {
        }

        // system name retrieved from the OpenVBDB database during a metadata search.
        QString openVGDBsystemName;

        // Displayed artwork url for frontend.
        QString frontArtwork;

        QString goodToolsCode;
        QString region;
        QString developer;
        QString releaseDate;
        QString genre;
        QString description;
        QString title;
        QString identifier;
        qreal progress;
        bool updated;

        // The romID value that is used as a UUID for the OpenVGDB for games.
        int romID;
    };

    QDebug operator<<( QDebug debug, const GameMetaData &entry );

    // A struct that attaches some basic metadata to a file path
    struct FileEntry {
            Q_GADGET
        public:
            FileEntry();

            FileEntry( const FileEntry &other );

            ~FileEntry() {}

            // Extra data that holds the result of the game scanner
            enum GameScannerResult{
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
            Q_ENUM( GameScannerResult )

            // Absolute path to a file
            QString filePath;

            // CRC32 hash for matching against game database
            QString crc32; // TODO: Use a more specific type to store the hash?

            // Do we have a hash cached?
            bool hasHashCached;

            // Database UUID, which in our case is the libretro system's name.
            QString gameUUID;

            // Some extensions may map to multiple systems, keep a list in order to present this info to the user later
            QStringList systemUUIDs;

            // The result of the game scanner. Defines which of the above members have valid values
            GameScannerResult scannerResult;

            // Metadata attached to the game file, like artwork.
            GameMetaData gameMetadata;
    };

    bool operator==( const FileEntry left, const FileEntry right );

    inline uint qHash( const FileEntry &key ) {
        return qHash( key.filePath );
    }

    using GameScannerResult = FileEntry::GameScannerResult;

    QDebug operator<<( QDebug debug, const FileEntry &entry );

    using FileList = QList<FileEntry>;

}

using namespace Library;
