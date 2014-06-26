
#ifndef VIDEO-GL_H
#define VIDEO-GL_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "core.h"


//! [1]
class GLWindow : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QUrl libcore READ libcore WRITE setCore NOTIFY libcoreChanged)
    Q_PROPERTY(QUrl game READ game WRITE setGame NOTIFY gameChanged)
    Q_PROPERTY(bool run READ run WRITE setRun NOTIFY runChanged)



public:
    GLWindow();
    ~GLWindow();

    qreal t() const { return m_t; }
    void setT( qreal t );
    void initShader();
    void initGL();
    void setCore( QUrl libcore );
    void setGame( QUrl game);
    void setRun( bool run );
    void setTexture( QOpenGLTexture::Filter min_scale,
                     QOpenGLTexture::Filter max_scale);

    QUrl libcore() const {
        return m_libcore;
    }

    QUrl game() const {
        return m_game;
    }

    bool run() const {
        return m_run;
    }

signals:
    void tChanged();
    void libcoreChanged();
    void gameChanged();
    void runChanged();

public slots:
    void paint();
    void cleanup();
    void sync();

private slots:
    void handleWindowChanged( QQuickWindow *win );

private:
    QOpenGLShaderProgram *m_program;
    QOpenGLTexture *m_texture;
    Core *core;

    qreal m_t;
    qreal m_thread_t;

    QUrl m_libcore;
    QUrl m_game;
    bool m_run;

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
//! [1]

#endif // VIDEO-GL_H
