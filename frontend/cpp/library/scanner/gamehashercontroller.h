#pragma once

#include "librarytypes.h"
#include "gamehasher.h"

#include <QObject>

class QThread;

class GameHasherController : public QObject {
        Q_OBJECT
        Q_PROPERTY( int progress READ progress WRITE setProgress NOTIFY progressChanged )
        Q_PROPERTY( bool running READ running WRITE setRunning NOTIFY runningChanged )

    public:
        explicit GameHasherController( QObject *parent = 0 );

        int progress() const;
        bool running() const
        {
            return mRunning;
        }

        void setRunning( const bool running ) {
            mRunning = running;
            emit runningChanged();
        }



    signals:

        void progressChanged();
        void runningChanged();

        // Scanning process complete
        void scanCompleted( FileList results );

        void filesNeedAssignment( FileList results );

        // Used to tell GameHasher to shut down on program exit
        void shutdownGameHasher();

    public slots:
        // Begins the scanning process. Once done, scanCompleted() be emitted with the results
        void scanForGames( QList<QUrl> urls );

        // Internal communication between GameHasher and GameHasherController
        void processResults( FileList results );

        void setProgress( const int progress );

    private:
        GameHasher *gameHasher;
        QThread *gameHasherThread;
        int mProgress;
        bool mRunning;
};

// Instantiate an singleton instance of GameHasherController for QML's use (needed to call static members)
QObject *GameHasherControllerSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine );
