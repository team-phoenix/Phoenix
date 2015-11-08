#ifndef LIBRARYWORKER_H
#define LIBRARYWORKER_H

#include "frontendcommon.h"

#include "phxpaths.h"
#include "metadatadatabase.h"
#include "libretrodatabase.h"
#include "logging.h"
#include "gamefileinfo.h"

#include "archivefileinfo.h"
#include "cuefileinfo.h"
#include "biosfileinfo.h"

namespace Library {

    struct GameData {
        qreal importProgress;
        QString system;
        QString timePlayed;
        QString title;
        QString filePath;
        QString crc32Checksum;

        QString artworkUrl;
        QString goodToolsCode;
        QString region;
        QString developer;
        QString releaseDate;
        QString genre;
        QString description;

        qint64 fileID;
    };

    class GameScanner : public QObject {
            Q_OBJECT
        public:

            explicit GameScanner( QObject *parent = 0 );
            ~GameScanner();

            // Synchronous state changers (thread-safe via built-in mutexes)
            bool insertCancelled();
            bool insertPaused();
            bool isRunning();
            bool resumeQuitScan();

            QString getResumeInsertID();
            QString getResumeDirectory();
            void setResumeInsertID( const QString id );
            void setResumeDirectory( const QString directory );
            void setResumeQuitScan( const bool resume );

        signals:
            void started();
            void finished();

            void insertGameData( const GameData gameData );
            void processFileInfo( QFileInfo fileInfo );
            void progressChanged( qreal value );


        public slots:
            void setInsertCancelled( const bool cancelled );
            void setInsertPaused( const bool paused );

            void eventLoopStarted();

            void handleDraggedUrls( QList<QUrl> urls );
            void handleContainsDrag( const bool contains );

            // These slots invoke the scanner to scan and import files (along with the constructor)
            int scanFolder( const QString path, bool autoStart );
            void handleDroppedUrls();


        private slots:
            void prepareGameData( QQueue<GameFileInfo> &queue );

        private:
            bool mInsertCancelled;
            bool mInsertPaused;
            QMutex mMutex;
            QQueue<GameFileInfo> mFileInfoQueue;
            bool mRunning;
            bool mContainsDrag;
            bool qmlResumeQuitScan;

            QList<QUrl> mDraggedUrls;

            QString mResumeInsertID;
            QString mResumeDirectory;

            // Setters
            void setIsRunning( const bool running );

            // Hash and enqueue all files within .zip if given a .zip file. Otherwise, enqueue a single file for processing
            void hashAndEnqueueFile( QString filePath );

    };

}


#endif // LIBRARYWORKER_H
