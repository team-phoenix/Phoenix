
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQuick/QQuickView>
#include <QQmlContext>

#ifdef Q_OS_LINUX
#include <pthread.h>
#include <signal.h>
#endif

#include "videoitem.h"
#include "core.h"
#include "gamelibrarymodel.h"
#include "librarydbmanager.h"


InputManager input_manager; // global

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    // When using QAudioOutput on linux/ALSA, we need to
    // block SIGIO on all threads except the audio thread
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGIO);
    pthread_sigmask(SIG_BLOCK, &set, nullptr);
#endif

    qSetMessagePattern("[%{if-debug}D%{endif}%{if-warning}W%{endif}"
                       "%{if-critical}C%{endif}%{if-fatal}F%{endif}]"
                       "%{if-category} [%{category}]:%{endif} %{message}");

    QGuiApplication a(argc, argv);
    a.setApplicationName("Phoenix");
    a.setApplicationVersion(PHOENIX_VERSION);
    a.setOrganizationName("Phoenix");
//    a.setOrganizationDomain("phoenix-emu.org");
    QSettings settings;


    qmlRegisterType<VideoItem>("VideoItem", 1, 0, "VideoItem");
    
    QQmlApplicationEngine engine;

    // first, set the context properties
    QQmlContext *rctx = engine.rootContext();
    GameLibraryModel gamelibr;
    rctx->setContextProperty("gamelibrary", &gamelibr);

    rctx->setContextProperty("inputmanager", &input_manager);

    // then, load qml and display the window
    engine.load(QUrl("qrc:/qml/main.qml"));
    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    QSurfaceFormat format = window->requestedFormat();

    if (settings.value("video/triple_buffering", false).toBool())
        format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    if (!settings.value("video/vsync", true).toBool())
        format.setSwapInterval(0);

    // format can't be changed once the window has been shown
    window->setFormat(format);

    window->show();

    return a.exec();
}
