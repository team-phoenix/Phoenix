
#include "openglwindow.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QImage>
#include "qdebug.h"

GLWindow::GLWindow()
    : m_program(0)
    , m_t(0)
    , m_thread_t(0)
    , m_texture(0)
    , m_libcore("")
    , m_game("")
{


//#ifdef Q_OS_WIN32
    //QString core_path = "../libretro-super/dist/win/bsnes_balanced_libretro.dll";
    //QString game_path = "../test_roms/Chrono Trigger (U) [!].smc";
//#endif

#ifdef Q_OS_LINUX
    QString core_path = "../libretro-super/dist/unix/bsnes_balanced_libretro.so";
    QString game_path = "../test_roms/Chrono Trigger (U) [!].smc";
#endif

    QString core_path = "C:/Users/robert/Desktop/leesstuff/bsnes_balanced_libretro.dll";
    QString game_path = "C:/Users/robert/Desktop/leesstuff/past.sfc";

    core = new Core();
    if (!core->loadCore(core_path)) {
        qDebug() << "Core was not loaded";
        exit(EXIT_FAILURE);
    }
    if (!core->loadGame(game_path)) {
        qDebug() << "Game was not loaded";
        exit(EXIT_FAILURE);
    }

    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

GLWindow::~GLWindow() {
    delete core;
    if (m_program)
        delete m_program;
    if (m_texture)
        delete m_texture;
}

void GLWindow::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void GLWindow::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection

        connect(win, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);

    }
}

void GLWindow::setCore( QUrl libcore ) {
    //core = temp_core;
    qDebug() << libcore.path(QUrl::PrettyDecoded);
    qDebug() << QString::fromUtf8(libcore.path().toUtf8());
    //core->loadCore(QString::fromUtf8(libcore.path().toUtf8()));
}

void GLWindow::setGame( QUrl game ) {
    qDebug() << game.path(QUrl::FullyEncoded);
    qDebug() << QString::fromUtf8(game.path().toUtf8());
}


void GLWindow::setRun( bool run ) {

    m_run = run;

}

void GLWindow::initGL() {

    qreal pixel_ratio = window()->devicePixelRatio();
    int w = int(pixel_ratio * window()->width());
    int h = int(pixel_ratio * window()->height());

    qreal window_aspect = (double)w / h;
    qreal desired_aspect = core->getAspectRatio();
    ulong corew = h * desired_aspect;
    ulong coreh = w / desired_aspect;

    if(fabsf(window_aspect - desired_aspect) < 0.0001f) {
        // no need
    } else if(window_aspect > desired_aspect) {
        glViewport( (w - corew) / 2, 0, corew, h );
    } else {
        glViewport( 0, (h - coreh) / 2, w, coreh );
    }

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

}

void GLWindow::initShader() {
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                       "attribute highp vec4 vertices;"
                                       "attribute mediump vec4 texCoord;"
                                       "varying mediump vec4 texc;"
                                       "void main() {"
                                       "    gl_Position = vertices;"
                                       "    texc = texCoord;"
                                       "}");
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                       "uniform sampler2D texture;"
                                       "varying mediump vec4 texc;"
                                       "void main() {"
                                       "    gl_FragColor = texture2D(texture, texc.st);"
                                       "}");

    // Binds location 0 to variable vertices
    m_program->bindAttributeLocation("vertices", 0);
    m_program->bindAttributeLocation("texCoord", 1);


    // Links vertex and frag shader
    m_program->link();

    connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
            this, SLOT(cleanup()), Qt::DirectConnection);

}

void GLWindow::setTexture( QOpenGLTexture::Filter min_scale, QOpenGLTexture::Filter max_scale ) {

    QImage::Format frame_format = retroToQImageFormat(core->getPixelFormat());

    m_texture = new QOpenGLTexture( QImage( ( const uchar * )core->getImageData(),
                                            core->getMinWidth(),
                                            core->getMinHeight(),
                                            core->getPitch(),
                                            frame_format ).mirrored() );

    m_texture->setMinMagFilters(min_scale, max_scale);

    m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);

}

void GLWindow::paint() {
    // Produces 1 frame of data
    if (m_run) {
        core->doFrame();

        // Sets viewport size, and enables / disables opengl functionality.
        initGL();

        if (!m_texture) {
            // sets texture from image
            setTexture( QOpenGLTexture::Linear, QOpenGLTexture::Nearest );
        }

        // Binds texture to opengl context
        m_texture->bind();

        if (!m_program) {
            // constructs vertex & frag shaders and links them.
            initShader();
        }


        // Makes this shader program the current shader affecting the window.
        m_program->bind();

        // Allows 0 to be used as variable id
        m_program->enableAttributeArray(0);
        m_program->enableAttributeArray(1);

        // Holds vertice values for triangle strip.
        // GLfloat is a cross platform macro for standard float
        // Triangle strip coords
        GLfloat values[] = {
            -1, -1,
            1, -1,
            -1, 1,
            1, 1
        };

        // Texture coords
        GLfloat texture[] = {
            0, 0,
            1, 0,
            0, 1,
            1, 1,
        };

        // Sets location 0 equal to vertices in values array
        m_program->setAttributeArray(0, QOpenGLTexture::Float32, values, 2);

        m_program->setAttributeArray(1, QOpenGLTexture::Float32, texture, 2);

        // Draws processed triangle stip onto the screen.
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Disables location 0
        m_program->disableAttributeArray(0);
        m_program->disableAttributeArray(1);
        m_program->release();
        m_texture->release();

        delete m_texture;
        m_texture = 0;

        // Loop forever;
        window()->update();

    }
}

void GLWindow::cleanup()
{
    // resets shader program
    if (m_program) {
        delete m_program;
        m_program = 0;
    }


}

void GLWindow::sync()
{
    m_thread_t = m_t;
}

