#include <QCoreApplication>
#include <QDebug>

#include <QtPlatformHeaders/QCocoaNativeContext>

#include <Cocoa/Cocoa.h>

void OSXSetSwapInterval( QVariant contextVariant, int interval ) {
    QCocoaNativeContext contextWrapper = contextVariant.value<QCocoaNativeContext>();
    NSOpenGLContext *context = contextWrapper.context();
    [ context setValues: &interval forParameter: NSOpenGLCPSwapInterval ];

    //    CGLContextObj ctx = static_cast<CGLContextObj>([context CGLContextObj]);
    //    CGLSetCurrentContext(ctx);

    //    // Get the data contained within the pixel format
    //    CGLPixelFormatObj cglPixelFormat = static_cast<CGLPixelFormatObj>(CGLGetPixelFormat(ctx));
    //    NSOpenGLPixelFormat *pixelFormat = [ [ NSOpenGLPixelFormat alloc ] initWithCGLPixelFormatObj: cglPixelFormat ];
    //    int doubleBuffered = -1;
    //    [ pixelFormat getValues: &doubleBuffered forAttribute: NSOpenGLPFADoubleBuffer forVirtualScreen: 0 ];
    //    int tripleBuffered = -1;
    //    [ pixelFormat getValues: &tripleBuffered forAttribute: NSOpenGLPFATripleBuffer forVirtualScreen: 0 ];
    //    [ pixelFormat release ];

    //    qDebug() << "double buffer: " << doubleBuffered;
    //    qDebug() << "triple buffer: " << tripleBuffered;
}
