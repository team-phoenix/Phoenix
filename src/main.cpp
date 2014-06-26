#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQuick/QQuickView>

#include "video-gl.h"
#include "core.h"

int main( int argc, char *argv[] ) {
    QGuiApplication a( argc, argv );
    
    qmlRegisterType<GLWindow>("GLWindow", 1, 0, "GLWindow");
    
    
    QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));
    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    window->show();

    return a.exec();
}
