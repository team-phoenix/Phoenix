#pragma once

#include <QMutex>
#include <QObject>
#include <QRect>

#include "keyboardstate.h"
#include "mousestate.h"
#include "node.h"
#include "phoenixwindow.h"

class QOpenGLContext;
class QOpenGLFramebufferObject;
class QThread;

/*
 * PhoenixWindowNode is a Node that wraps around an underlying PhoenixWindow. It fires heartbeat commands whenever the
 * window emits a QQuickWindow::frameSwapped() signal and will tell the underlying window whenever VSync mode changes.
 *
 * It handles creating and setting up a shared QOpenGLContext for the dynamic pipeline's use. Since the dynamic
 * pipeline and the scene graph's OpenGL context are created at different times, we must check that both are good to go
 * before sending any loads down the dynamic pipeline.
 *
 * Warning: The dynamic pipeline OpenGL context and the core must live in the same thread!
 */

class PhoenixWindowNode : public Node {
        Q_OBJECT
        Q_PROPERTY( PhoenixWindow *phoenixWindow MEMBER phoenixWindow NOTIFY phoenixWindowChanged )

    public:
        explicit PhoenixWindowNode( Node *parent = 0 );
        ~PhoenixWindowNode();
        PhoenixWindow *phoenixWindow{ nullptr };

        // Sends the context out iff not done this session, dynamic pipeline and OpenGL context is ready
        void checkIfCommandsShouldFire();

    signals:
        void phoenixWindowChanged( PhoenixWindow *phoenixWindow );

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;

        void frameSwapped();
        void geometryChanged();

        void mousePressd( QPointF point, Qt::MouseButtons buttons );
        void mouseReleased( QPointF point, Qt::MouseButtons buttons );
        void mouseMoved( QPointF point, Qt::MouseButtons buttons );
        void keyPressed( int key );
        void keyReleased( int key );

    private:
        // A copy of the game thread, used to push the context and its FBO to it
        QThread *gameThread { nullptr };

        // True if we already told the dynamic pipeline the GL context for this session
        bool firedOpenGLContextCommand { false };

        // Is there a load pending that we can't emit because we don't have an OpenGL context ready yet?
        bool fireLoad { false };

        // Framerate of the monitor this Window exists in
        // TODO: Use whatever techniques are out there to get a more accurate number
        qreal hostFPS{ 60.0 };

        // Window geometry
        QRect geometry;

        // Keyboard/mouse producer stuff

        // Holds the keyboard state as it's being built
        KeyboardState keyboard;

        // Circular buffer of keyboard states
        KeyboardState keyboardBuffer[ 100 ];
        int keyboardBufferIndex { 0 };

        // Ensure reads/writes to keyboardBuffer are atomic
        QMutex mutex;

        // Helpers
        void insertState( int key, bool state );

        // Circular buffer of mouse states
        MouseState mouseBuffer[ 100 ];
        int mouseBufferIndex { 0 };

        MouseState mouse;
};
