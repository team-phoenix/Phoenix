
#ifndef VIDEO-GL_H
#define VIDEO-GL_H

#include <QtQuick/QQuickItem>
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QOpenGLTexture>
#include <QImage>
#include <QByteArray>

#include "qdebug.h"
#include "core.h"
#include "audio.h"

class GLWindow : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(QUrl libcore READ libcore WRITE setCore NOTIFY libcoreChanged)
    Q_PROPERTY(QUrl game READ game WRITE setGame NOTIFY gameChanged)
    Q_PROPERTY(bool run READ run WRITE setRun NOTIFY runChanged)
    Q_PROPERTY(double sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged)

public:
    GLWindow();
    ~GLWindow();

    void initShader();
    void initGL();
    void setCore( QUrl libcore );
    void setGame( QUrl game);
    void setRun( bool run );
    void setSampleRate(double sampleRate );
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

    double sampleRate() const {
        return m_sample_rate;
    }

signals:
    void libcoreChanged();
    void gameChanged();
    void runChanged();
    void sampleRateChanged();

public slots:
    void paint();
    void cleanup();

private slots:
    void handleWindowChanged( QQuickWindow *win );

private:
    QOpenGLShaderProgram *m_program;
    QOpenGLTexture *m_texture;
    Core *core;

    QUrl m_libcore;
    QUrl m_game;
    bool m_run;

    // Audio
    Audio *audio;
    double m_sample_rate;

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

#endif // VIDEO-GL_H
