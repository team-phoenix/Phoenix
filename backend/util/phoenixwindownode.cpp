#include "phoenixwindownode.h"

#include <QScreen>
#include <QSurface>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>

#include <QThread>
#include "logging.h"

PhoenixWindowNode::PhoenixWindowNode( Node *parent ) : Node( parent ) {
    connect( this, &PhoenixWindowNode::phoenixWindowChanged, this, [ & ]( PhoenixWindow * phoenixWindow ) {
        if( phoenixWindow ) {
            this->phoenixWindow = phoenixWindow;
            phoenixWindow->phoenixWindowNode = this;
            connect( phoenixWindow, &QQuickWindow::frameSwapped, this, &PhoenixWindowNode::frameSwapped );
            connect( phoenixWindow, &QQuickWindow::xChanged, this, &PhoenixWindowNode::geometryChanged );
            connect( phoenixWindow, &QQuickWindow::yChanged, this, &PhoenixWindowNode::geometryChanged );
            connect( phoenixWindow, &QQuickWindow::widthChanged, this, &PhoenixWindowNode::geometryChanged );
            connect( phoenixWindow, &QQuickWindow::heightChanged, this, &PhoenixWindowNode::geometryChanged );

            // Keyboard/mouse input
            connect( phoenixWindow, &PhoenixWindow::keyPressed, this, &PhoenixWindowNode::keyPressed );
            connect( phoenixWindow, &PhoenixWindow::keyReleased, this, &PhoenixWindowNode::keyReleased );
            connect( phoenixWindow, &PhoenixWindow::mousePressed, this, &PhoenixWindowNode::mousePressd );
            connect( phoenixWindow, &PhoenixWindow::mouseReleased, this, &PhoenixWindowNode::mouseReleased );
            connect( phoenixWindow, &PhoenixWindow::mouseMoved, this, &PhoenixWindowNode::mouseMoved );
        }
    } );
}

PhoenixWindowNode::~PhoenixWindowNode() {
}

// Public

void PhoenixWindowNode::checkIfCommandsShouldFire() {
    qDebug() << "Check" << !firedOpenGLContextCommand << ( phoenixWindow && ( phoenixWindow->dynamicPipelineContext != nullptr ) ) << fireLoad << ( gameThread != nullptr );

    // Check if it's time to tell the dynamic pipeline about the context and the load
    if( !firedOpenGLContextCommand && phoenixWindow && phoenixWindow->dynamicPipelineContext && fireLoad && gameThread ) {
        firedOpenGLContextCommand = true;

        // Move context to the game thread
        phoenixWindow->dynamicPipelineContext->moveToThread( gameThread );
        phoenixWindow->dynamicPipelineSurface->moveToThread( gameThread );

        // Send everything from this class out
        emit commandOut( Command::SetWindowGeometry, geometry, nodeCurrentTime() );
        emit commandOut( Command::SetSurface, QVariant::fromValue<QOffscreenSurface *>( phoenixWindow->dynamicPipelineSurface ), nodeCurrentTime() );
        emit commandOut( Command::SetOpenGLContext, QVariant::fromValue<QOpenGLContext *>( phoenixWindow->dynamicPipelineContext ), nodeCurrentTime() );
        emit commandOut( Command::Load, QVariant(), nodeCurrentTime() );
    }
}

// Public slots

void PhoenixWindowNode::commandIn( Node::Command command, QVariant data, qint64 timeStamp ) {
    switch( command ) {
        case Command::Load: {
            qDebug() << command;
            Q_ASSERT( phoenixWindow );

            // Don't emit load until the dynamic pipeline context is ready
            fireLoad = true;
            checkIfCommandsShouldFire();
            break;
        }

        // Reset all the checkers
        case Command::Unload: {
            emit commandOut( command, data, timeStamp );

            // Reset this so it may be fired again next load
            firedOpenGLContextCommand = false;

            // Reset this so a new load can be called later
            fireLoad = false;
            break;
        }

        case Command::SetGameThread: {
            qDebug() << command;
            emit commandOut( command, data, timeStamp );

            gameThread = data.value<QThread *>();
            checkIfCommandsShouldFire();
            break;
        }

        case Command::HandleDynamicPipelineReady: {
            qDebug() << command;
            Q_ASSERT( phoenixWindow );
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::SetVsync: {
            Q_ASSERT( phoenixWindow );
            emit commandOut( command, data, timeStamp );
            phoenixWindow->setVsync( data.toBool() );
            break;
        }

        default: {
            emit commandOut( command, data, timeStamp );
            break;
        }
    }
}

void PhoenixWindowNode::frameSwapped() {
    // Send keyboard state on its way
    {
        // Copy keyboard into buffer
        mutex.lock();
        keyboardBuffer[ keyboardBufferIndex ] = keyboard;
        mutex.unlock();

        // Send buffer on its way
        emit dataOut( DataType::KeyboardInput, &mutex, &keyboardBuffer[ keyboardBufferIndex ], 0, nodeCurrentTime() );

        // Increment the index
        keyboardBufferIndex = ( keyboardBufferIndex + 1 ) % 100;

        // Reset the keyboard
        keyboard.head = 0;
        keyboard.tail = 0;
    }

    // Send mouse state on its way
    {
        // Copy mouse into buffer
        mutex.lock();
        mouseBuffer[ mouseBufferIndex ] = mouse;
        mutex.unlock();

        // Send buffer on its way
        emit dataOut( DataType::MouseInput, &mutex, &mouseBuffer[ mouseBufferIndex ], 0, nodeCurrentTime() );

        // Increment the index
        mouseBufferIndex = ( mouseBufferIndex + 1 ) % 100;
    }

    emit commandOut( Command::Heartbeat, QVariant(), nodeCurrentTime() );
}

void PhoenixWindowNode::geometryChanged() {
    QRect newGeometry = phoenixWindow->geometry();

    if( newGeometry != geometry ) {
        geometry = newGeometry;
        emit commandOut( Command::SetWindowGeometry, geometry, nodeCurrentTime() );
    }
}

void PhoenixWindowNode::mousePressd( QPointF point , Qt::MouseButtons buttons ) {
    mouse.position = point;
    mouse.buttons = buttons;
}

void PhoenixWindowNode::mouseReleased( QPointF point, Qt::MouseButtons buttons ) {
    mouse.position = point;
    mouse.buttons = buttons;
}

void PhoenixWindowNode::mouseMoved( QPointF point, Qt::MouseButtons buttons ) {
    mouse.position = point;
    mouse.buttons = buttons;
}

void PhoenixWindowNode::keyPressed( int key ) {
    insertState( key, true );
}

void PhoenixWindowNode::keyReleased( int key ) {
    insertState( key, false );
}

// Private

void PhoenixWindowNode::insertState( int key, bool state ) {
    // Insert key and state into next location
    keyboard.key[ keyboard.tail ] = key;
    keyboard.pressed[ keyboard.tail ] = state;

    // Recalculate head and tail
    keyboard.tail = ( keyboard.tail + 1 ) % 128;

    // Advance head forward if tail will overwrite it next time
    if( keyboard.tail == keyboard.head ) {
        keyboard.head = ( keyboard.head + 1 ) % 128;
    }
}
