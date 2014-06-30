
#ifndef VIDEO_GL_H
#define VIDEO_GL_H

#include <QtQuick/QQuickItem>
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QOpenGLTexture>
#include <QImage>
#include <QByteArray>
#include <QEvent>

#include "qdebug.h"
#include "core.h"
#include "audio.h"

class GLWindow : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(QString libcore READ libcore WRITE setCore NOTIFY libcoreChanged)
    Q_PROPERTY(QString game READ game WRITE setGame NOTIFY gameChanged)
    Q_PROPERTY(bool run READ run WRITE setRun NOTIFY runChanged)
    Q_PROPERTY(QString windowVisibility READ windowVisibility WRITE setWindowVisibility NOTIFY windowVisibilityChanged)

public:
    GLWindow();
    ~GLWindow();

    void initShader();
    void initGL();
    void setCore( QString libcore );
    void setGame( QString game);
    void setRun( bool run );
    void setWindowVisibility( QString windowVisibility );
    void setTexture( QOpenGLTexture::Filter min_scale,
                     QOpenGLTexture::Filter max_scale);

    QString libcore() const {
        return m_libcore;
    }

    QString game() const {
        return m_game;
    }

    bool run() const {
        return m_run;
    }

    QString windowVisibility() const {
        return m_win_visibility;
    }


protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void geometryChanged(const QRectF &newGeom, const QRectF &oldGeom) Q_DECL_OVERRIDE {
        Q_UNUSED(newGeom);
        Q_UNUSED(oldGeom);
        QQuickItem::geometryChanged(newGeom, oldGeom);
        refreshItemGeometry();
    };

signals:
    void libcoreChanged( QString );
    void gameChanged( QString );
    void runChanged( bool );
    void windowVisibilityChanged( QString windowVisibility );

public slots:
    void paint();
    void cleanup();

private slots:
    void handleWindowChanged( QQuickWindow *win );
    void handleGeometryChanged(int unused) {
        Q_UNUSED(unused);
        refreshItemGeometry();
    }
    void handleSceneGraphInitialized() {
        refreshItemGeometry();
    }

private:
    // Video
    // [1]
    QOpenGLShaderProgram *m_program;
    QOpenGLTexture *m_texture;
    Core *core;
    int frame_count;
    int item_w;
    int item_h;
    qreal item_aspect; // item aspect ratio
    QPoint viewportXY;
    // [1]

    // Qml defined variables
    // [2]
    QString m_libcore;
    QString m_game;
    QString m_win_visibility;
    bool m_run;
    //[2]

    // Audio
    //[3]
    Audio *audio;
    void updateAudioFormat();
    //[3]

    // Input
    // [4]
    unsigned id;
    unsigned device;
    unsigned port;
    bool is_pressed;
    uint32_t index;
    //[4]

    void refreshItemGeometry(); // called every time the item's with/height/x/y change

    static inline QImage::Format retroToQImageFormat( enum retro_pixel_format fmt ) {
        static QImage::Format format_table[3] = {
            QImage::Format_RGB16,   // RETRO_PIXEL_FORMAT_0RGB1555
            QImage::Format_RGB32,   // RETRO_PIXEL_FORMAT_XRGB8888
            QImage::Format_RGB16    // RETRO_PIXEL_FORMAT_RGB565
        };

        if(fmt >= 0 && fmt < (sizeof(format_table)/sizeof(QImage::Format))) {
            return format_table[fmt];
        }
        return QImage::Format_Invalid;
    }

};

#endif // VIDEO_GL_H
