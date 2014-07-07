
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQuick/QQuickView>

#ifdef Q_OS_LINUX
#include <pthread.h>
#include <signal.h>
#endif

#include "videoitem.h"
#include "core.h"

int main( int argc, char *argv[] ) {
#ifdef Q_OS_LINUX
    // When using QAudioOutput on linux/ALSA, we need to
    // block SIGIO on all threads except the audio thread
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGIO);
    pthread_sigmask(SIG_BLOCK, &set, nullptr);
#endif
    QGuiApplication a( argc, argv );
    
    qmlRegisterType<VideoItem>("VideoItem", 1, 0, "VideoItem");
    
    
    QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));
    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    window->show();

    return a.exec();
}
