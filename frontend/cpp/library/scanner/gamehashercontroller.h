#pragma once

#include "librarytypes.h"
#include "gamehasher.h"

#include <QObject>

class QThread;

class GameHasherController : public QObject {
        Q_OBJECT

    public:
        explicit GameHasherController( QObject *parent = 0 );

    signals:
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

    private:
        GameHasher *gameHasher;
        QThread *gameHasherThread;
};

// Instantiate an singleton instance of GameHasherController for QML's use (needed to call static members)
QObject *GameHasherControllerSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine );
