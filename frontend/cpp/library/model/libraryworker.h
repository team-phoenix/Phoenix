#ifndef LIBRARYWORKER_H
#define LIBRARYWORKER_H

#include "frontendcommon.h"

#include "phxpaths.h"
#include "metadatadatabase.h"
#include "libretrodatabase.h"
#include "logging.h"
#include "gamefileinfo.h"

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

    class LibraryWorker : public QObject {
            Q_OBJECT
        public:

            explicit LibraryWorker( QObject *parent = 0 );
            ~LibraryWorker();

            bool insertCancelled();
            bool insertPaused();
            bool isRunning();
            bool resumeQuitScan();

            QString resumeInsertID();
            QString resumeDirectory();

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

            bool findGameFiles( const QString localUrl, bool autoStart );

            void eventLoopStarted();

            void handleDraggedUrls( QList<QUrl> urls );
            void handleDroppedUrls();
            void handleContainsDrag( const bool contains );


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

            void enqueueFiles( QString &filePath );

    };

}


#endif // LIBRARYWORKER_H
