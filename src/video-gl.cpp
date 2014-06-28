
#include "video-gl.h"

#include "audio.h"

GLWindow::GLWindow() :
          m_program(0), m_texture(0),
          m_libcore(""), m_game("") {


//#ifdef Q_OS_WIN32
    //QString core_path = "../libretro-super/dist/win/bsnes_balanced_libretro.dll";
    //QString game_path = "../test_roms/Chrono Trigger (U) [!].smc";
//#endif

//#ifdef Q_OS_LINUX
    //QString core_path = "../libretro-super/dist/unix/bsnes_balanced_libretro.so";
    //QString game_path = "../test_roms/Chrono Trigger (U) [!].smc";
//#endif

    core = new Core();
    //if (!core->loadCore(core_path)) {
       // qDebug() << "Core was not loaded";
        //exit(EXIT_FAILURE);
    //}
    //if (!core->loadGame(game_path)) {
        //qDebug() << "Game was not loaded";
        //exit(EXIT_FAILURE);
    //}

    audio = new Audio();
    connect(this, SIGNAL(runChanged(bool)), audio, SLOT(runChanged(bool)));
    Q_CHECK_PTR(audio);
    audio->start();
    core->aio = audio->aio();


    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

GLWindow::~GLWindow() {
    delete core;
    if (m_program)
        delete m_program;
    if (m_texture)
        delete m_texture;

}

void GLWindow::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection

        connect(win, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);

    }
}

void GLWindow::setCore( QString libcore ) {
    qDebug() << "Core: " << libcore;
    if ( !core->loadCore(libcore.toStdString().c_str() )) {
        qDebug() << "Core was not loaded";
        exit(EXIT_FAILURE);
    }
    emit libcoreChanged(libcore);
}

void GLWindow::setGame( QString game ) {
    qDebug() << "Game: " << game;
    if ( !core->loadGame(game.toStdString().c_str() )) {
        qDebug() << "Core was not loaded";
        exit(EXIT_FAILURE);
    }
    updateAudioFormat();
    emit gameChanged(game);
}


void GLWindow::setRun( bool run ) {
    m_run = run;
    emit runChanged(run);
}

void GLWindow::updateAudioFormat() {
    QAudioFormat format;
    format.setSampleSize(16);
    format.setSampleRate(core->getSampleRate());
    format.setChannelCount(2);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    // TODO test format
    audio->setFormat(format);
}

void GLWindow::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Escape:
            qDebug() << "Esc pressed";
            break;
        case Qt::Key_Return:
            qDebug() << "Return pressed";
            break;
        case Qt::Key_Space:
            qDebug() << "Space pressed";
            break;
        case Qt::Key_Left:
            qDebug() << "Left pressed";
            break;
        case Qt::Key_Right:
            qDebug() << "Right pressed";
            break;
        case Qt::Key_Down:
            qDebug() << "Down pressed";
            break;
        case Qt::Key_Up:
            qDebug() << "Up pressed";
            break;
        case Qt::Key_A:
            qDebug() << "A pressed";
            break;
        case Qt::Key_S:
            qDebug() << "S pressed";
            break;
        case Qt::Key_W:
            qDebug() << "W pressed";
            break;
        case Qt::Key_D:
            qDebug() << "D pressed";
            break;
        default:
            qDebug() <<  "Key not handled";
            break;
    }
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
    }
    else if(window_aspect > desired_aspect) {
        glViewport( (w - corew) / 2, 0, corew, h );
    }
    else {
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

    }

    if (!m_texture) {
        // sets texture from image
        setTexture( QOpenGLTexture::Linear, QOpenGLTexture::Nearest );

    }

    // Sets viewport size, and enables / disables opengl functionality.
    initGL();

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

    //int16_t audio_data[] = { core->getLeftChannel(), core->getRightChannel() };

    //audio->play( QByteArray( (const char *)audio_data, sizeof(audio_data) ) );

    //audio->play( core->getAudioData(), core->getAudioFrames() );

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->release();
    m_texture->release();

    if (m_run) {
        delete m_texture;
        m_texture = 0;
    }

    // Loop forever;
    window()->update();
}

void GLWindow::cleanup()
{
    // resets shader program
    if (m_program) {
        delete m_program;
        m_program = 0;
    }


}
