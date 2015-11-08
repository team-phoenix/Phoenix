#ifndef GAMEFILEINFO_H
#define GAMEFILEINFO_H

#include "frontendcommon.h"

#include "libretrodatabase.h"
#include "metadatadatabase.h"

namespace Library {

    class GameFileInfo : public QFileInfo {
        public:
            GameFileInfo( const QString &file );

            struct HeaderData {
                QString result;
                QString system;
                qint64 seekPosition;
                qint64 byteLength;
            };

            // FIXME: Unused
            struct CueData {
                QString system;
                QString sha1;
            };

            QString system() const;
            QString crc32CheckSum() const;
            QString title() const;
            QString fullFilePath() const;
            QString timePlayed() const;
            QString crc32Checksum() const;
            QString artworkUrl() const;

            enum FileType {
                UnsupportedFile = -1,
                GameFile = 0,
                ZipFile,
                BiosFile,
                CueFile,
            };

            FileType fileType() const;

            // Returns list of extensions the scanner supports based off of the system database
            static QStringList gameFilter();

            HeaderData getPossibleHeaders( const QStringList &possibleSystems );

            void cache( const QString &location );

            void scanOpenVGDBForGame();

        protected:
            FileType mFileType;
            QString mSystem;
            QStringList mPossibleSystemsList;
            QString mCrc32Checksum;
            QString mTitle;
            QString mFullFilePath;
            QString mArtworkUrl;
            QSqlQuery mLibretroQuery;

            // Query the systems database for all systems (Phoenix UUIDs) that use our extension
            QStringList getSystemListForExtension( const QString &extension );

            // We internally store hashes as QStrings for convenience
            QString getCRC32AsQString( const QString &filePath );

            // Calculate and store this file's hash and determine what systems it might apply to based on extension
            virtual void fillBasicInfo();

            bool isBios( QString &biosName );

            // Query OpenVGDB for metadata based on this file's hash. If that fails, query based off filename
            void fillMetadataFromOpenVGDB( int romID, QSqlQuery &openVGDBQuery );

        private:
            QString mTimePlayed;

            // Helper for debugging errors with SQL queries
            // http://stackoverflow.com/a/10641002/4190028
            static QString getLastExecutedQuery( const QSqlQuery &query );

    };


}

#endif // GAMEFILEINFO_H
