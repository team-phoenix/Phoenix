
#include "videoitem.h"

VideoItem::VideoItem() {

    core = new Core();

    m_program = 0;
    m_texture = 0;
    m_libcore = "";
    m_game = "";
    m_gamepad_scan = false;

    sdl_joystick = new SDLJoystick(this);
    sdl_joystick->onScan();
    sdl_joystick->onStart(20);

    id = 0;
    device = RETRO_DEVICE_JOYPAD;
    port = 0;
    is_pressed = true;
    index = 0;

    audio = new Audio();
    Q_CHECK_PTR(audio);
    audio->start();
    core->audio_buf = audio->abuf();

    connect(&fps_timer, SIGNAL(timeout()), this, SLOT(updateFps()));
    frame_timer.invalidate();

    connect(sdl_joystick, SIGNAL(dataChanged(unsigned, unsigned, unsigned, unsigned)), this, SLOT(processGamePad(unsigned, unsigned, unsigned, unsigned)));
    connect(sdl_joystick, SIGNAL(dataChanged(bool, unsigned, unsigned, unsigned, unsigned)), this, SLOT(processGamePad(bool, unsigned, unsigned, unsigned, unsigned)));
    connect(this, SIGNAL(runChanged(bool)), audio, SLOT(runChanged(bool)));
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

VideoItem::~VideoItem() {
    delete core;
    sdl_joystick->deleteLater();
    if (m_program)
        delete m_program;
    if (m_texture)
        delete m_texture;
}

void VideoItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection

        connect(win, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);
        connect(win, SIGNAL(widthChanged(int)), this, SLOT(handleGeometryChanged(int)));
        connect(win, SIGNAL(heightChanged(int)), this, SLOT(handleGeometryChanged(int)));
        connect(win, SIGNAL(sceneGraphInitialized()), this, SLOT(handleSceneGraphInitialized()));

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);

    }
}

void VideoItem::handleSceneGraphInitialized() {
    refreshItemGeometry();
    // initialize m_texture with an empty 1x1 black image
    QImage emptyImage(1, 1, QImage::Format_RGB32);
    emptyImage.fill(Qt::black);
    m_texture = new QOpenGLTexture(emptyImage);
}

void VideoItem::setWindowVisibility(QString windowVisibility) {

    m_win_visibility = windowVisibility;
    emit windowVisibilityChanged( windowVisibility );

}

void VideoItem::setSystemDirectory(QString systemDirectory) {

    core->setSystemDirectory(systemDirectory);

}

void VideoItem::processGamePad(unsigned port, unsigned device, unsigned index, unsigned id) {

    core->setInputStateCallBack(port, device, index, id);

}

void VideoItem::processGamePad(bool is_pressed, unsigned port, unsigned device, unsigned index, unsigned id) {

    core->setInputStateCallBack(is_pressed, port, device, index, id);

}

void VideoItem::setGamePadScan(bool gamepadScan) {
    m_gamepad_scan = gamepadScan;
    emit gamepadScanChanged(gamepadScan);
}

void VideoItem::setCore( QString libcore ) {
    qCDebug(phxVideo) << "Loading core:" << libcore;
    if (!core->loadCore(libcore.toStdString().c_str())) {
        qCCritical(phxVideo, "Couldn't load core !");
//        exit(EXIT_FAILURE);
    }
    emit libcoreChanged(libcore);
}

void VideoItem::setGame( QString game ) {
    qCDebug(phxVideo) << "Loading game:" << game;
    if (!core->loadGame(game.toStdString().c_str())) {
        qCCritical(phxVideo, "Couldn't load game !");
//        exit(EXIT_FAILURE);
    }
    updateAudioFormat();
    emit gameChanged(game);
}


void VideoItem::setRun( bool run ) {
    m_run = run;
    if (run) {
        qCDebug(phxVideo, "Started");
        fps_timer.start(1000);
    } else {
        qCDebug(phxVideo, "Paused");
    }
    emit runChanged(run);
}

void VideoItem::updateAudioFormat() {
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

void VideoItem::keyEvent(QKeyEvent *event) {

    id = 16;
    device = RETRO_DEVICE_JOYPAD;
    port = 0;
    is_pressed = (event->type() == QEvent::KeyPress) ? true : false;
    index = 0;

    switch(event->key()) {
        case Qt::Key_Escape:
            if(is_pressed)
                emit windowVisibilityChanged("Windowed");
            break;
        case Qt::Key_Space:
            if(is_pressed) {
                if (m_run)
                    setRun(false);
                else
                    setRun(true);
            }
            break;
        case Qt::Key_Return:
            id = RETRO_DEVICE_ID_JOYPAD_START;
            break;
        case Qt::Key_Left:
            id = RETRO_DEVICE_ID_JOYPAD_LEFT;
            break;
        case Qt::Key_Right:
            id = RETRO_DEVICE_ID_JOYPAD_RIGHT;
            break;
        case Qt::Key_Down:
            id = RETRO_DEVICE_ID_JOYPAD_DOWN;
            break;
        case Qt::Key_Up:
            id = RETRO_DEVICE_ID_JOYPAD_UP ;
            break;
        case Qt::Key_A:
            id = RETRO_DEVICE_ID_JOYPAD_A;
            break;
        case Qt::Key_S:
            id = RETRO_DEVICE_ID_JOYPAD_B;
            break;
        case Qt::Key_W:
            break;
        case Qt::Key_D:
            break;
        case Qt::Key_X:
            id = RETRO_DEVICE_ID_JOYPAD_X;
            break;
        case Qt::Key_Z:
            id = RETRO_DEVICE_ID_JOYPAD_Y;
            break;
        default:
            if(is_pressed)
                qDebug() << "Key not handled";
            break;
    }

    core->setInputStateCallBack(is_pressed, port, device, index, id);

}

void VideoItem::refreshItemGeometry() {
    qreal pixel_ratio = window()->devicePixelRatio();
    item_w = int(pixel_ratio * width());
    item_h = int(pixel_ratio * height());
    item_aspect = (qreal)item_w / item_h;

    viewportXY = mapToScene(QPointF(x(), height()+y())).toPoint();
    viewportXY.setY(window()->height() - viewportXY.y());
}

void VideoItem::initGL() {

    qreal desired_aspect = core->getAspectRatio();
    ulong core_w = item_h * desired_aspect;
    ulong core_h = item_w / desired_aspect;
    QRect viewportRect;

    if(fabsf(item_aspect - desired_aspect) < 0.0001f) {
        viewportRect.setRect(viewportXY.x(), viewportXY.y(), core_w, core_h);
    }
    else if(item_aspect > desired_aspect) {
        viewportRect.setRect(viewportXY.x() + ((item_w - core_w) / 2),
                             viewportXY.y(),
                             core_w, item_h);
    }
    else {
        viewportRect.setRect(viewportXY.x(),
                             viewportXY.y() + ((item_h - core_h) / 2),
                             item_w, core_h );
    }

    glViewport(viewportRect.x(), viewportRect.y(),
               viewportRect.width(), viewportRect.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

}

void VideoItem::initShader() {
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

void VideoItem::setTexture( QOpenGLTexture::Filter min_scale, QOpenGLTexture::Filter max_scale ) {


    QImage::Format frame_format = retroToQImageFormat(core->getPixelFormat());

    m_texture->destroy();
    m_texture->setData( QImage( ( const uchar * )core->getImageData(),
                        core->getBaseWidth(),
                        core->getBaseHeight(),
                        core->getPitch(),
                        frame_format ).mirrored() );

    m_texture->setMinMagFilters(min_scale, max_scale);

    m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);

}

inline bool VideoItem::limitFps() {
    qreal target_fps_interval = round(1000000.0 / core->getFps()); // µsec
    if (!frame_timer.isValid()) {
        frame_timer.start();
        return false;
    }

    qint64 last_frame_time = frame_timer.nsecsElapsed() / (qint64)1000;
    if (frame_measures.size() == 10)
        frame_measures.removeFirst();
    frame_measures.append(last_frame_time);

    qint64 frames_time = 0;
    foreach (qint64 fm, frame_measures) {
       frames_time += fm;
    }

    if (target_fps_interval * 10 > frames_time * 1.05) {
        return true;
    }

    frame_timer.start();

    return false;
}

void VideoItem::paint() {

    if (m_run && !limitFps()) {
        core->doFrame();
        fps_count++;

        // Sets texture from core->getImageData();
        setTexture( QOpenGLTexture::Linear, QOpenGLTexture::LinearMipMapNearest );
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

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->release();
    m_texture->release();

    // Loop forever;
    window()->update();
}

void VideoItem::cleanup()
{
    // resets shader program
    if (m_program) {
        delete m_program;
        m_program = nullptr;
    }
    if (m_texture) {
        delete m_texture;
        m_texture = nullptr;
    }
}
