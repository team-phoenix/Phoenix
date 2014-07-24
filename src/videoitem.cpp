
#include "videoitem.h"



VideoItem::VideoItem()
{
    core = new Core();

    m_program = nullptr;
    texture_node = nullptr;
    m_libcore = "";

    audio = new Audio();
    Q_CHECK_PTR(audio);
    audio->start();
    core->audio_buf = audio->abuf();
    m_volume = 0.0;

    keyboard = new Keyboard();
    core->getInputManager()->append(keyboard);

    connect(&fps_timer, SIGNAL(timeout()), this, SLOT(updateFps()));
    frame_timer.invalidate();
    fps_deviation = 0;

    connect(this, SIGNAL(runChanged(bool)), audio, SLOT(runChanged(bool)));
    connect(this, SIGNAL(volumeChanged(qreal)), audio, SLOT(setVolume(qreal)));
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

VideoItem::~VideoItem()
{
    delete keyboard;
    delete core;
    if (m_program)
        delete m_program;
    if (texture_node)
        delete texture_node;
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

void VideoItem::handleSceneGraphInitialized()
{
    refreshItemGeometry();
    // initialize texture_node with an empty 1x1 black image
    QImage emptyImage(1, 1, QImage::Format_RGB32);
    emptyImage.fill(Qt::black);
    texture_node = window()->createTextureFromImage(emptyImage);
}

void VideoItem::setWindowed(bool windowVisibility)
{

    m_set_windowed = windowVisibility;
    emit setWindowedChanged( windowVisibility );

}

void VideoItem::setVolume(qreal volume)
{
    m_volume = volume;
    emit volumeChanged(volume);

}

void VideoItem::setSystemDirectory(QString systemDirectory)
{
    m_system_directory = systemDirectory;
    core->setSystemDirectory(systemDirectory);

}

void VideoItem::setSaveDirectory(QString saveDirectory)
{
    m_save_directory = saveDirectory;
    core->setSaveDirectory(saveDirectory);

}

void VideoItem::saveGameState()
{
    QFileInfo info(m_game);
    if (m_game != "" && m_libcore != "")
        core->saveGameState(m_save_directory, info.baseName());

}

void VideoItem::loadGameState()
{
    QFileInfo info(m_game);
    if (core->loadGameState(m_save_directory, info.baseName())) {
        qDebug() << "Save State loaded";
    }
}

void VideoItem::setCore(QString libcore)
{
    if (libcore == "")
        return;
    qCDebug(phxVideo) << "Loading core:" << libcore;
    if (!core->loadCore(libcore.toStdString().c_str())) {
        qCCritical(phxVideo, "Couldn't load core !");
//        exit(EXIT_FAILURE);
    }

    const retro_system_info *i = core->getSystemInfo();
    qCDebug(phxVideo) << "Loaded core" << i->library_name << i->library_version;
    emit libcoreChanged(libcore);
}

void VideoItem::setGame(QString game)
{
    if (game == "")
        return;
    m_game = game;
    qCDebug(phxVideo) << "Loading game:" << game;
    if (!core->loadGame(game.toStdString().c_str())) {
        qCCritical(phxVideo, "Couldn't load game !");
//        exit(EXIT_FAILURE);
    }
    qCDebug(phxVideo, "Loaded game at %ix%i @ %.2ffps", core->getBaseWidth(),
            core->getBaseHeight(), core->getFps());
    updateAudioFormat();
    emit gameChanged(game);
}


void VideoItem::setRun(bool run)
{
    m_run = run;
    if (run) {
        qCDebug(phxVideo, "Core started");
        fps_timer.start(1000);
    } else {
        qCDebug(phxVideo, "Core paused");
    }
    emit runChanged(run);
}

void VideoItem::updateAudioFormat()
{
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

void VideoItem::keyEvent(QKeyEvent *event)
{
    bool is_pressed = (event->type() == QEvent::KeyPress) ? true : false;

    switch(event->key()) {
        case Qt::Key_Escape:
            if(is_pressed)
                emit setWindowedChanged(true);
            break;
        case Qt::Key_Space:
            if(is_pressed) {
                if (m_run)
                    setRun(false);
                else
                    setRun(true);
            }
            break;
    }

    // we also pass every KeyEvent to each connected InputDevice which is a Keyboard.
    // a bit ugly, but this avoid overhead of signal/slots and event filters
    QList<InputDevice *> devices = core->getInputManager()->getDevices();
    for (int i=0; i < devices.size(); ++i) {
        auto keyboardinput = dynamic_cast<Keyboard *>(devices.at(i));
        if (keyboardinput != nullptr)
            keyboardinput->processKeyEvent(event);;
    }
}

void VideoItem::refreshItemGeometry()
{
    qreal pixel_ratio = window()->devicePixelRatio();
    item_w = int(pixel_ratio * width());
    item_h = int(pixel_ratio * height());
    item_aspect = (qreal)item_w / item_h;

    viewportXY = mapToScene(QPointF(x(), height()+y())).toPoint();
    viewportXY.setY(window()->height() - viewportXY.y());
}

void VideoItem::initGL()
{
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

void VideoItem::initShader()
{
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

void VideoItem::setTexture(QSGTexture::Filtering filter)
{
    QImage::Format frame_format = retroToQImageFormat(core->getPixelFormat());

    texture_node->deleteLater();
    texture_node = window()->createTextureFromImage(QImage((const uchar *)core->getImageData(),
                                                        core->getBaseWidth(),
                                                        core->getBaseHeight(),
                                                        core->getPitch(),
                                                        frame_format).mirrored()
                                                    , QQuickWindow::TextureOwnsGLTexture);

    texture_node->setFiltering(filter);

    texture_node->setHorizontalWrapMode(QSGTexture::ClampToEdge);
    texture_node->setVerticalWrapMode(QSGTexture::ClampToEdge);

}

inline bool VideoItem::limitFps()
{
    qreal target_fps_interval = round(1000000.0 / core->getFps()); // µsec
    if (!frame_timer.isValid()) {
        frame_timer.start();
        return false;
    }

    qint64 last_frame_time = frame_timer.nsecsElapsed() / (qint64)1000;
    if (fps_deviation < (-target_fps_interval * 20) && last_frame_time > target_fps_interval) {
        // reset fps_deviation if we are more than 20 frames late
        fps_deviation = 0;
    }
    fps_deviation += target_fps_interval - last_frame_time;

    frame_timer.start();

    // if we deviated from the core's clock so much that we
    // are one full frame ahead, skip a frame.
    if(fps_deviation > target_fps_interval) {
        fps_deviation -= target_fps_interval;
        return true;
    }

    return false;
}

void VideoItem::paint()
{
    if (m_run && !limitFps()) {
        core->doFrame();
        fps_count++;

        // Sets texture from core->getImageData();
        setTexture(QSGTexture::Linear);
    }


    // Sets viewport size, and enables / disables opengl functionality.
    initGL();

    // Binds texture to opengl context
    texture_node->bind();

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
    if (texture_node) {
        delete texture_node;
        texture_node = nullptr;
    }
}
