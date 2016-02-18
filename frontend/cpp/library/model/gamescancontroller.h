#ifndef GAMESCANCONTROLLER_H
#define GAMESCANCONTROLLER_H

/*
#include "frontendcommon.h"

#include "futuremanager.h"
#include "archivefile.h"


using WatcherPtr = QFutureWatcher<QVariant> *;

class GameScanController : public QObject {
        Q_OBJECT
    public:
        explicit GameScanController( QObject *parent = 0 );

        // Error code that could be emitted by "handleScanStarted()".
        // Used in the "error( const ErrorCode )" signal.
        enum ErrorCode {
            PathError = 0, // Path could not be found
            TypeError = 1, // Tried to use a file as a folder.
        };

        // Getters
        int progress();
        QStringList scanPath() const;

    signals:
        void scanPathChanged();

        void threadSafeStartScan();

        void progressChanged();

        // emitted when the scanning process is completely done
        void scanFinished();

        // emitted when the scanning process has first started,
        // before any actual work has been completed.
        void scanStarted();

        // Is used to check whether any error has occured in the scanning process.
        void error( const ErrorCode errorCode );

    public slots:
        // Starts the scanning process in a thread safe manner.
        // Calls "emit threadSafeStartScan()".
        void startScan();

        // Setters that have to be connected to a proxy class.
        void appendScanPath( const QString scanPath );
        void setProgress( int progress );

    private slots:
        // Connects to the "error( const ErrorCode ) signal
        // and gives a read out of what each error means.
        void handleError( const ErrorCode error );

        // Connects to the signal "threadSafeStartScan()" in order to
        // do the actual scanning work.
        void handleScanStarted();

        // Slots connected to the QFutureWatcher's
        void handleEnumerateFilesFinished();
        void handleParseFilesFinished();
        void handleParseCueFilesFinished();

        // Parse files asynchronous
        void parseArchiveFiles( QStringList result );
        void parseCueFiles( QStringList result );

    private:
        QStringList mScanPathList;
        QList<WatcherPtr> mWatcherList;
        int mProgress;

        inline QVariantList watcherResult();
};

*/

#endif // GAMESCANCONTROLLER_H
