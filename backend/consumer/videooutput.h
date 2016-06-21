#pragma once

#include "node.h"

#include <QQuickItem>
#include <QOpenGLContext>
#include <QQmlParserStatus>

class QMutex;
class QSGTexture;

/*
 * VideoOutput is a QQuickItem that consumes video data. It is meant to be instantiated from QML.
 */

class VideoOutput : public QQuickItem {
        Q_OBJECT

        Q_PROPERTY( int aspectMode MEMBER aspectMode WRITE setAspectMode NOTIFY aspectModeChanged )
        Q_PROPERTY( qreal aspectRatio MEMBER aspectRatio NOTIFY aspectRatioChanged )
        Q_PROPERTY( bool linearFiltering MEMBER linearFiltering NOTIFY linearFilteringChanged )
        Q_PROPERTY( bool television MEMBER television WRITE setTelevision NOTIFY televisionChanged )
        Q_PROPERTY( bool ntsc MEMBER ntsc WRITE setNtsc NOTIFY ntscChanged )
        Q_PROPERTY( bool widescreen MEMBER widescreen WRITE setWidescreen NOTIFY widescreenChanged )

    public:
        explicit VideoOutput( QQuickItem *parent = nullptr );
        ~VideoOutput();

        void setState( Node::State state );
        void setFormat( LibretroVideoFormat consumerFmt );
        void data( QMutex *mutex, void *data, size_t bytes, qint64 timestamp );

        void setTextureID( GLuint textureID );

        // Setters for the properties, will force a recheck of the aspect ratio if any are called
        void setAspectMode( int aspectMode );
        void setTelevision( bool television );
        void setNtsc( bool ntsc );
        void setWidescreen( bool widescreen );

        // The mutex used by the core to draw video
        // We'll need to lock it during 3D rendering
        QMutex *mutex { nullptr };

        void classBegin() override;
        void componentComplete() override;

    signals:
        // Properties
        void aspectModeChanged();
        void aspectRatioChanged();
        void linearFilteringChanged();
        void televisionChanged();
        void ntscChanged();
        void widescreenChanged();

    private:
        // Current state. Used to ignore data that arrives after already being told we're no longer playing
        Node::State state{ Node::State::Stopped };
        LibretroVideoFormat format;

        // The framebuffer that holds the latest frame from Core
        uchar *framebuffer { nullptr };
        size_t framebufferSize { 0 };

        // Holds a pointer to the framebuffer via its underlying QImage, used by renderer to upload framebuffer to GPU
        QSGTexture *texture { nullptr };

        // The texture name used by 3d cores
        GLuint textureID { 0 };

        // Called by render thread whenever it's time to render and needs an update from us
        // We'll assign the current texture to the stored node given to us, creating this stored node if it does not
        // already exist
        // Main thread (our thread) is guarantied to be suspended while this function executes, the rest run normally
        // paintData is a pointer to a QSGTransformNode which contains the transformation matrix (unused)
        QSGNode *updatePaintNode( QSGNode *storedNode, UpdatePaintNodeData * ) override;

        // Properties

        // The mode to scale the picture in
        int aspectMode { 0 };

        // The correct aspect ratio to display this picture in
        qreal aspectRatio { 1.0 };
        qreal calculateAspectRatio( LibretroVideoFormat format );

        // Linear vs nearest-neighbor filtering
        bool linearFiltering{ false };

        // Controls whether to use analog television mode or not (false = digital TV or handheld games, etc... anything
        // with a square PAR)
        bool television{ false };

        // Do we expect 480 vertical lines (NTSC, true) or 576 vertical lines (PAL, false)?
        // Ignored if television is false
        bool ntsc{ true };

        // Is this standard 4:3 (false) or is the image anamorphic widescreen (16:9 packed into a 4:3 frame, true)?
        // Ignored if television is false
        bool widescreen{ false };

        // Has this mutex been locked by us?
        bool lockedByUs { false };

        // Discovered from: http://stackoverflow.com/a/96035/4190028
        // Find rational approximation to given real number
        // By: David Eppstein / UC Irvine / 8 Aug 1993
        // With corrections from Arno Formella, May 2008
        QPair<int, int> doubleToFraction( double x, long maxden );
};
