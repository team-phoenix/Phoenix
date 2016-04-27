#pragma once

#include "librarytypes.h"
#include "gamehasher.h"

#include <QObject>

class QThread;

class GameHasherController : public QObject {
        Q_OBJECT
        Q_PROPERTY( int progress READ progress WRITE setProgress NOTIFY progressChanged )
        Q_PROPERTY( bool running READ running WRITE setRunning NOTIFY runningChanged )
        Q_PROPERTY( bool paused READ paused NOTIFY pausedChanged )

    public:
        explicit GameHasherController( QObject *parent = 0 );

        int progress() const;
        bool running() const;
        bool paused() const;

        void setRunning( bool running );
        void setPaused( bool paused );

    signals:

        void progressChanged();
        void runningChanged();
        void pausedChanged();

        // Scanning process complete
        void scanCompleted( FileList results );

        void filesNeedAssignment( FileList results );

        // Used to tell GameHasher to shut down on program exit
        void shutdownGameHasher();

    public slots:
        // Begins the scanning process. Once done, scanCompleted() be emitted with the results
        void scanForGames( QList<QUrl> urls );

        void setProgress( int progress );

        // Pauses the current scan, and waits for a resume.
        void pause();


        // Resumes a paused scan.
        void resume();

        // Cancels and discards the current scan completely.
        void cancel();

    private:
        GameHasher *gameHasher;
        QThread *gameHasherThread;
        int mProgress;
        bool mRunning;
        bool mPaused;
};

// Instantiate an singleton instance of GameHasherController for QML's use (needed to call static members)
QObject *GameHasherControllerSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine );
