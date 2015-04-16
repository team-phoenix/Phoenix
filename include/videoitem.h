
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
#include <QSGSimpleTextureNode>

#include "qdebug.h"
#include "core.h"
#include "audio.h"
#include "keyboard.h"
#include "logging.h"

/* Think of VideoItem as a QML Rectangle, that has its texture constantly changing.
 * It's exposed to QML, as the VideoItem type, and is instantiated from inside of the
 * GameView.qml file.
 *
 * The VideoItem class also limits the frame rate of QML, if a game is supposed to
 * be run at a lower frame rate than 60.
 *
 * Internally, this class acts as the controller for the libretro core, Core, and the audio output controller, Audio.
 * This essentially makes it a libretro frontend in the form of a QML item.
 */

class VideoItem : public QQuickItem {
        Q_OBJECT

        Q_PROPERTY( QString libcore READ libcore WRITE setCore NOTIFY libcoreChanged )
        Q_PROPERTY( QString game READ game WRITE setGame NOTIFY gameChanged )
        Q_PROPERTY( bool isRunning READ isRunning WRITE setRun NOTIFY runChanged )
        Q_PROPERTY( bool setWindowed READ setWindowed WRITE setWindowed NOTIFY setWindowedChanged )
        Q_PROPERTY( QString systemDirectory READ systemDirectory WRITE setSystemDirectory NOTIFY systemDirectoryChanged )
        Q_PROPERTY( int fps READ fps NOTIFY fpsChanged )
        Q_PROPERTY( qreal volume READ volume WRITE setVolume NOTIFY volumeChanged )
        Q_PROPERTY( int filtering READ filtering WRITE setFiltering NOTIFY filteringChanged )
        Q_PROPERTY( bool stretchVideo READ stretchVideo WRITE setStretchVideo NOTIFY stretchVideoChanged )
        Q_PROPERTY( qreal aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged )


    public:
        VideoItem();
        ~VideoItem();

        void initShader();
        void initGL();
        void setCore( QString libcore );
        void setGame( QString game );
        void setRun( bool isRunning );
        void setWindowed( bool setWindowed );
        void setSystemDirectory( QString systemDirectory );
        void setTexture();
        void setVolume( qreal volume );
        void setFiltering( int filtering );
        void setStretchVideo( bool stretchVideo );
        void setAspectRatio( qreal aspectRatio );


        QString libcore() const {
            return m_libcore;
        }

        QString game() const {
            return m_game;
        }

        bool isRunning() const {
            return m_run;
        }

        bool setWindowed() const {
            return m_set_windowed;
        }

        QString systemDirectory() const {
            return m_system_directory;
        }

        int fps() const {
            return m_fps;
        }

        qreal volume() const {
            return m_volume;
        }

        int filtering() const {
            return m_filtering;
        }

        bool stretchVideo() const {
            return m_stretch_video;
        }

        qreal aspectRatio() const {
            return m_aspect_ratio;
        }




    protected:
        void keyEvent( QKeyEvent *event );
        void keyPressEvent( QKeyEvent *event ) override {
            keyEvent( event );
        };
        void keyReleaseEvent( QKeyEvent *event ) override {
            keyEvent( event );
        };
        QSGNode *updatePaintNode( QSGNode *, UpdatePaintNodeData * );

    signals:
        void libcoreChanged( QString );
        void gameChanged( QString );
        void runChanged( bool );
        void setWindowedChanged( bool );
        void systemDirectoryChanged();
        void saveDirectoryChanged();
        void fpsChanged( int );
        void volumeChanged( qreal );
        void filteringChanged();
        void stretchVideoChanged();
        void aspectRatioChanged();

    public slots:
        //void paint();
        void saveGameState();
        void loadGameState();
        QStringList getAudioDevices();


    private slots:
        void handleWindowChanged( QQuickWindow *win );
        void handleGeometryChanged( int unused ) {
            Q_UNUSED( unused );
            refreshItemGeometry();
        }
        void handleSceneGraphInitialized();
        void updateFps() {
            m_fps = fps_count * ( 1000.0 / fps_timer.interval() );
            fps_count = 0;
            emit fpsChanged( m_fps );
        }


    private:
        // Video
        // [1]
        QSGTexture *texture;
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
        qreal m_aspect_ratio;
        // [1]

        // Qml defined variables
        // [2]
        QString m_system_directory;
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

        static inline QImage::Format retroToQImageFormat( enum retro_pixel_format fmt ) {
            static QImage::Format format_table[3] = {
                QImage::Format_RGB16,   // RETRO_PIXEL_FORMAT_0RGB1555
                QImage::Format_RGB32,   // RETRO_PIXEL_FORMAT_XRGB8888
                QImage::Format_RGB16    // RETRO_PIXEL_FORMAT_RGB565
            };

            if( fmt >= 0 && fmt < ( sizeof( format_table ) / sizeof( QImage::Format ) ) ) {
                return format_table[fmt];
            }

            return QImage::Format_Invalid;
        }

};

#endif // VIDEOITEM_H
