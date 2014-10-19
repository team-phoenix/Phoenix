
#include "videoitem.h"


extern InputManager input_manager;

VideoItem::VideoItem()
{
    core = new Core();

    texture = nullptr;
    m_libcore = "";
    m_stretch_video = false;
    m_filtering = 2;
    m_aspect_ratio = 0.0;
    m_fps = 0;

    audio = new Audio();
    Q_CHECK_PTR(audio);
    audio->start();
    core->audio_buf = audio->abuf();
    m_volume = 1.0;

    connect(&fps_timer, SIGNAL(timeout()), this, SLOT(updateFps()));
    frame_timer.invalidate();
    fps_deviation = 0;
    fps_count = 0;

    connect(this, SIGNAL(runChanged(bool)), audio, SLOT(runChanged(bool)));
    connect(this, SIGNAL(volumeChanged(qreal)), audio, SLOT(setVolume(qreal)));
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

VideoItem::~VideoItem()
{
    unload();
    delete core;
    if (texture)
        texture->deleteLater();
}

void VideoItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection
    setFlag(QQuickItem::ItemHasContents, true);
        connect(win, SIGNAL(frameSwapped()), this, SLOT(update()));
        connect(win, SIGNAL(widthChanged(int)), this, SLOT(handleGeometryChanged(int)));
        connect(win, SIGNAL(heightChanged(int)), this, SLOT(handleGeometryChanged(int)));
        connect(win, SIGNAL(sceneGraphInitialized()), this, SLOT(handleSceneGraphInitialized()));

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        //win->setClearBeforeRendering(false);

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

void VideoItem::handleSceneGraphInitialized()
{
    refreshItemGeometry();
    // initialize texture_node with an empty 1x1 black image
    QImage emptyImage(1, 1, QImage::Format_RGB32);
    emptyImage.fill(Qt::black);
    texture = window()->createTextureFromImage(emptyImage);
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

void VideoItem::setAspectRatio(qreal aspectRatio)
{
    m_aspect_ratio = aspectRatio;
    emit aspectRatioChanged();
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
    m_libcore = libcore;
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

void VideoItem::setFiltering(int filtering)
{
    m_filtering = filtering;
    emit filteringChanged();
}

void VideoItem::setStretchVideo(bool stretchVideo)
{
    m_stretch_video = stretchVideo;
    emit stretchVideoChanged();
}

QStringList VideoItem::getAudioDevices()
{
    QStringList list;
    foreach (const QAudioDeviceInfo &device_info, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        list.append(device_info.deviceName());
    return list;
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
            if(is_pressed) {
                emit setWindowedChanged(true);
                event->accept();
            }
            break;
        case Qt::Key_Space:
            if(is_pressed) {
                setRun(m_run ? false : true);
                event->accept();
            }
            break;
    }
}




void VideoItem::setTexture()
{
    QImage::Format frame_format = retroToQImageFormat(core->getPixelFormat());

    if (texture)
        texture->deleteLater();
    texture = window()->createTextureFromImage(QImage((const uchar *)core->getImageData(),
                                                        core->getBaseWidth(),
                                                        core->getBaseHeight(),
                                                        core->getPitch(),
                                                        frame_format).mirrored()
                                                    , QQuickWindow::TextureOwnsGLTexture);

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

void VideoItem::cleanup()
{
    if (texture) {
        texture->deleteLater();
        texture = nullptr;
    }
}

QSGNode *VideoItem::updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *paint_data)
{
    Q_UNUSED(paint_data)

    if (!aspectRatio())
        setAspectRatio(core->getAspectRatio());

    if (run() && !limitFps()) {
        core->doFrame();
        fps_count++;

        // Sets texture from core->getImageData();
        setTexture();
    }

    QSGSimpleTextureNode *tex_node = nullptr;
    if (old_node) {
        tex_node = static_cast<QSGSimpleTextureNode *>(old_node);
    }
    else {
        tex_node = new QSGSimpleTextureNode();
    }

    tex_node->setTexture(texture);
    tex_node->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
    tex_node->setRect(boundingRect());
    tex_node->setFiltering(static_cast<QSGTexture::Filtering>(filtering()));

    return tex_node;

}

void VideoItem::unload()
{
    core->unload();
}

