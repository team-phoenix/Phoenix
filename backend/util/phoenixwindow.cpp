/*
 * Copyright © 2016 athairus
 *
 * This file is part of Phoenix.
 *
 * Phoenix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "phoenixwindow.h"

#include <QDebug>
#include <QEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMetaObject>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QQuickWindow>
#include <QSurfaceFormat>
#include <QtGlobal>
#include <QThread>
#include <QWindow>

#include <memory>

#include "logging.h"
#include "phoenixwindownode.h"

void SceneGraphHelper::setVSync( QQuickWindow *window, QOpenGLContext *context, bool vsync ) {
    context->makeCurrent( window );

#if defined( Q_OS_WIN )

    void ( *wglSwapIntervalEXT )( int ) = nullptr;

    if( ( wglSwapIntervalEXT =
              reinterpret_cast<void ( * )( int )>( context->getProcAddress( "wglSwapIntervalEXT" ) ) ) ) {
        wglSwapIntervalEXT( vsync ? 1 : 0 );
    } else {
        qWarning() << "Couldn't resolve wglSwapIntervalEXT. Unable to change VSync settings.";
    }

#elif defined( Q_OS_MACX )

    // Call a helper to execute the necessary Objective-C code
    extern void OSXSetSwapInterval( QVariant context, int interval );
    OSXSetSwapInterval( context->nativeHandle(), vsync ? 1 : 0 );

#elif defined( Q_OS_LINUX )

    void ( *glxSwapIntervalEXT )( int ) = nullptr;

    if( ( glxSwapIntervalEXT =
              reinterpret_cast<void ( * )( int )>( context->getProcAddress( "glxSwapIntervalEXT" ) ) ) ) {
        glxSwapIntervalEXT( vsync ? 1 : 0 );
    } else {
        qWarning() << "Couldn't resolve glxSwapIntervalEXT. Unable to change VSync settings.";
    }

#endif
}

PhoenixWindow::PhoenixWindow( QQuickWindow *parent ) : QQuickWindow( parent ),
    dynamicPipelineSurface( new QOffscreenSurface() ), sceneGraphHelper( new SceneGraphHelper() ) {

    // As soon as the scene graph is initialized we can set up the surface, context and FBO
    connect( this, &QQuickWindow::openglContextCreated, this, [ & ]( QOpenGLContext * context ) {
        qDebug() << "Scene graph context ready" << QThread::currentThread() << context->thread();

        // Allocate an OpenGL context controller
        dynamicPipelineContext = new QOpenGLContext();

        // Grab the main window's format now that it's fully resolved
        QSurfaceFormat format = context->format();
        qDebug() << format;

        // Match the context and surface's format to the main window's format
        dynamicPipelineSurface->setFormat( format );
        dynamicPipelineContext->setFormat( format );

        // Create the surface now
        dynamicPipelineSurface->create();

        // Share the context with the QSG's context
        // This allows the FBO's texture (created automatically by QOpenGLFramebufferObject's constructor) to be
        // shared between two contexts. Use mutexes to ensure access by either context (they live on separate
        // threads) is atomic
        dynamicPipelineContext->setShareContext( context );

        // Set the alpha buffer size
        //format.setAlphaBufferSize( 8 );

        // Create the context and make it current
        dynamicPipelineContext->create();
        dynamicPipelineContext->makeCurrent( dynamicPipelineSurface );

        // Set our clear color
        dynamicPipelineContext->functions()->glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

        // Not really necessary but good practice
        dynamicPipelineContext->doneCurrent();

        // Make PhoenixWindowNode check if we're good to go
        if( phoenixWindowNode ) {
            phoenixWindowNode->checkIfCommandsShouldFire();
        }
    } );

    connect( this, &QQuickWindow::sceneGraphInitialized, this, [ & ]() {
        QThread *sceneGraphThread = openglContext()->thread();
        qDebug() << "Scene graph ready" << QThread::currentThread() << sceneGraphThread;
        sceneGraphIsInitialized = true;
        sceneGraphHelper->moveToThread( sceneGraphThread );
    } );

    // Grab window surface format as the OpenGL context will not be created yet
    QSurfaceFormat fmt = format();

    // Enforce the default value for vsync
    fmt.setSwapInterval( vsync ? 1 : 0 );

#if defined( Q_OS_WIN )

#elif defined( Q_OS_MACX )

    // For proper OS X fullscreen
    setFlags( flags() | Qt::WindowFullscreenButtonHint );

#endif
    //    qDebug() << fmt;
    //    qDebug() << format();
    //    qDebug() << QSurfaceFormat::defaultFormat();
    //    qDebug() << QSurfaceFormat();

    // Apply it, will be set when the context is created
    setFormat( fmt );

    //    qDebug() << format();
    update();
    qDebug() << Q_FUNC_INFO;
}

PhoenixWindow::~PhoenixWindow() {
    delete sceneGraphHelper;
    dynamicPipelineContext->deleteLater();
    dynamicPipelineSurface->deleteLater();
}

bool PhoenixWindow::event( QEvent *event ) {
    switch( event->type() ) {
        case QEvent::KeyPress: {
            //qDebug() << "press" << static_cast<QKeyEvent *>( event )->text() << static_cast<QKeyEvent *>( event )->modifiers();
            emit keyPressed( static_cast<QKeyEvent *>( event )->key() );
            break;
        }

        case QEvent::KeyRelease: {
            //qDebug() << "release" << static_cast<QKeyEvent *>( event )->text() << static_cast<QKeyEvent *>( event )->modifiers();
            emit keyReleased( static_cast<QKeyEvent *>( event )->key() );
            break;
        }

        case QEvent::MouseButtonPress: {
            emit mousePressed( static_cast<QMouseEvent *>( event )->windowPos(), static_cast<QMouseEvent *>( event )->buttons() );
            break;
        }

        case QEvent::MouseButtonRelease: {
            emit mouseReleased( static_cast<QMouseEvent *>( event )->windowPos(), static_cast<QMouseEvent *>( event )->buttons() );
            break;
        }

        case QEvent::MouseMove: {
            emit mouseMoved( static_cast<QMouseEvent *>( event )->windowPos(), static_cast<QMouseEvent *>( event )->buttons() );
            break;
        }

        default:
            break;
    }

    return QQuickWindow::event( event );
}

void PhoenixWindow::setVsync( bool vsync ) {
    if( this->vsync == vsync ) {
        return;
    }

    // TODO: Cache the change so it'll get applied when it is ready
    if( !sceneGraphIsInitialized ) {
        return;
    }

    this->vsync = vsync;

    QMetaObject::invokeMethod( sceneGraphHelper, "setVSync", Qt::QueuedConnection, Q_ARG( QQuickWindow *, this ),
                               Q_ARG( QOpenGLContext *, openglContext() ), Q_ARG( bool, vsync ) );

    return;
}
