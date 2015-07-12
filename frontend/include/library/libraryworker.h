#ifndef LIBRARYWORKER_H
#define LIBRARYWORKER_H

#include <QObject>
#include <QMutex>
#include <QFileInfo>
#include <QUrl>
#include <QQueue>
#include <QDirIterator>

#include "metadatadatabase.h"

namespace Library {

    struct GameData {
        qreal importProgress;
        QString system;
        QString timePlayed;
        QString title;
        QString filePath;
        QString sha1;

        QString artworkUrl;
        QString goodToolsCode;
        QString region;
        QString developer;
        QString releaseDate;
        QString genre;
        QString description;

        bool updated = false;
        bool start = false;
    };

    class LibraryWorker : public QObject {
            Q_OBJECT
        public:

            explicit LibraryWorker( QObject *parent = 0 );
            ~LibraryWorker();

            bool insertCancelled();
            bool insertPaused();

            bool isRunning();

        signals:
            void started();
            void finished();

            void insertGameData( const GameData gameData );
            void processFileInfo( QFileInfo fileInfo );
            void progressChanged( qreal value );


        public slots:
            void setInsertCancelled( const bool cancelled );
            void setInsertPaused( const bool paused );

            void prepareMetadata( GameData &gameData );
            void findGameFiles( const QUrl url );

        private slots:
            void prepareGameData( QQueue<QFileInfo> &queue );

        private:
            bool mInsertCancelled;
            bool mInsertPaused;
            QMutex mMutex;
            QStringList mFileFilters;
            QQueue<QFileInfo> mFileInfoQueue;
            QQueue<QDirIterator> mDirectoryQueue;
            bool mRunning;
            int mCurrentCount;

            MetaDataDatabase mMetaDatabase;

            // Setters
            void setIsRunning( const bool running );

            // Helper Functions
            void checkHeaderOffsets( const QFileInfo &fileInfo, QString &platform );
            bool getCueFileInfo( QFileInfo &fileInfo );
            QString getCheckSum( const QString filePath );
    };

}


#endif // LIBRARYWORKER_H
