
#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QtQuick/QQuickItem>
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QOpenGLTexture>
#include <QImage>
#include <QWindow>
#include <QByteArray>
#include <QSGTexture>
#include <QEvent>

#include "qdebug.h"
#include "core.h"
#include "audio.h"
#include "keyboard.h"
#include "logging.h"


class VideoItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(QString libcore READ libcore WRITE setCore NOTIFY libcoreChanged)
    Q_PROPERTY(QString game READ game WRITE setGame NOTIFY gameChanged)
    Q_PROPERTY(bool run READ run WRITE setRun NOTIFY runChanged)
    Q_PROPERTY(bool setWindowed READ setWindowed WRITE setWindowed NOTIFY setWindowedChanged)
    Q_PROPERTY(QString systemDirectory READ systemDirectory WRITE setSystemDirectory NOTIFY systemDirectoryChanged)
    Q_PROPERTY(QString saveDirectory READ saveDirectory  WRITE setSaveDirectory NOTIFY systemDirectoryChanged)
    Q_PROPERTY(int fps READ fps NOTIFY fpsChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int filtering READ filtering WRITE setFiltering NOTIFY filteringChanged)
    Q_PROPERTY(bool stretchVideo READ stretchVideo WRITE setStretchVideo NOTIFY stretchVideoChanged)


public:
    VideoItem();
    ~VideoItem();

    void initShader();
    void initGL();
    void setCore(QString libcore);
    void setGame(QString game);
    void setRun(bool run );
    void setWindowed(bool setWindowed);
    void setSystemDirectory(QString systemDirectory);
    void setSaveDirectory(QString saveDirectory);
    void setTexture();
    void setVolume(qreal volume);
    void setFiltering(int filtering);
    void setStretchVideo(bool stretchVideo);


    QString libcore() const
    {
        return m_libcore;
    }

    QString game() const
    {
        return m_game;
    }

    bool run() const
    {
        return m_run;
    }

    bool setWindowed() const
    {
        return m_set_windowed;
    }

    QString systemDirectory() const
    {
        return m_system_directory;
    }

    QString saveDirectory() const
    {
        return m_save_directory;
    }

    int fps() const
    {
        return m_fps;
    }

    qreal volume() const
    {
        return m_volume;
    }

    int filtering() const
    {
        return m_filtering;
    }

    bool stretchVideo() const
    {
        return m_stretch_video;
    }




protected:
    void keyEvent(QKeyEvent *event);
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE {
        keyEvent(event);
    };
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE {
        keyEvent(event);
    };
    void geometryChanged(const QRectF &newGeom, const QRectF &oldGeom) Q_DECL_OVERRIDE {
        Q_UNUSED(newGeom);
        Q_UNUSED(oldGeom);
        QQuickItem::geometryChanged(newGeom, oldGeom);
        refreshItemGeometry();
    };

signals:
    void libcoreChanged(QString);
    void gameChanged(QString);
    void runChanged(bool);
    void setWindowedChanged(bool);
    void systemDirectoryChanged();
    void saveDirectoryChanged();
    void fpsChanged(int);
    void volumeChanged(qreal);
    void filteringChanged();
    void stretchVideoChanged();

public slots:
    void paint();
    void cleanup();
    void saveGameState();
    void loadGameState();
    QStringList getAudioDevices();

private slots:
    void handleWindowChanged(QQuickWindow *win);
    void handleGeometryChanged(int unused) {
        Q_UNUSED(unused);
        refreshItemGeometry();
    }
    void handleSceneGraphInitialized();
    void updateFps() {
        m_fps = fps_count * (1000.0 / fps_timer.interval());
        fps_count = 0;
        emit fpsChanged(m_fps);
    }


private:
    // Video
    // [1]
    QOpenGLShaderProgram *m_program;
    QSGTexture *texture_node;
    Core *core;
    int item_w;
    int item_h;
    qreal item_aspect; // item aspect ratio
    QPoint viewportXY;
    int fps_count;
    QTimer fps_timer;
    QElapsedTimer frame_timer;
    qint64 fps_deviation;
    int m_filtering;
    bool m_stretch_video;
    // [1]

    // Qml defined variables
    // [2]
    QString m_system_directory;
    QString m_save_directory;
    QString m_libcore;
    QString m_game;
    bool m_set_windowed;
    bool m_run;
    int m_fps;
    qreal m_volume;
    //[2]

    // Audio
    //[3]
    Audio *audio;
    void updateAudioFormat();
    //[3]

    void refreshItemGeometry(); // called every time the item's with/height/x/y change

    bool limitFps(); // return true if it's too soon to ask for another frame

    static inline QImage::Format retroToQImageFormat(enum retro_pixel_format fmt) {
        static QImage::Format format_table[3] = {
            QImage::Format_RGB16,   // RETRO_PIXEL_FORMAT_0RGB1555
            QImage::Format_RGB32,   // RETRO_PIXEL_FORMAT_XRGB8888
            QImage::Format_RGB16    // RETRO_PIXEL_FORMAT_RGB565
        };

        if(fmt >= 0 && fmt < (sizeof(format_table) / sizeof(QImage::Format))) {
            return format_table[fmt];
        }
        return QImage::Format_Invalid;
    }

};

#endif // VIDEOITEM_H
