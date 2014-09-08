#include <phoenixwindow.h>

PhoenixWindow::PhoenixWindow()
{
    m_surface_format = requestedFormat();
    pressed = false;
    resize_w = false;
    resize_h = false;
    drag = false;
    click_x = 0;
    click_y = 0;
    m_frameless = false;
    // Fix a bug that appears when you leave the software with alt + f4 when it is in fullscreen. This blocks the software fullscreen
    //this->show();

}

PhoenixWindow::~PhoenixWindow()
{

}

void PhoenixWindow::setWindowScreen(QScreen *screen)
{
    setScreen(screen);
}

void PhoenixWindow::setWinSwapBehavior(int behavior) {
    m_swap_behavior = behavior;
    QSurfaceFormat::SwapBehavior _behavior = static_cast<QSurfaceFormat::SwapBehavior>(behavior);
    m_surface_format.setSwapBehavior(_behavior);

    setWinFormat();
    emit swapBehaviorChanged(behavior);
}

void PhoenixWindow::setWinSwapInterval(int interval)
{
    m_swap_interval = interval;
    m_surface_format.setSwapInterval(interval);

    setWinFormat();
    emit swapIntervalChanged(interval);

}

void PhoenixWindow::setWinFormat()
{
    setFormat(m_surface_format);
}

void PhoenixWindow::setFrameless(bool frameless)
{
    m_frameless = frameless;
    if (frameless)
        setFlags(Qt::FramelessWindowHint);
    emit framelessChanged();
}


void PhoenixWindow::mousePressEvent(QMouseEvent *event)
{
    if (!frameless()) {
        QQuickWindow::mousePressEvent(event);
        return;
    }

    if (event->button() != Qt::LeftButton)
        event->ignore();
    else {
        pressed = true;
        click_x = event->x();
        click_y = event->y();
        qDebug() << click_y;
        if (click_y > 10 && click_y  < 55)
            drag = true;
        else if (click_y < 10)
            resize_h = true;
        if (click_x < 10)
            resize_w = true;

    }
    QQuickWindow::mousePressEvent(event);

}

void PhoenixWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (!frameless()) {
        QQuickWindow::mouseReleaseEvent(event);
        return;
    }
    pressed = false;
    drag = false;
    resize_w = false;
    resize_h = false;
    click_x = 0;
    click_y = 0;

    QQuickWindow::mouseReleaseEvent(event);
}

void PhoenixWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!frameless()) {
        QQuickWindow::mouseMoveEvent(event);
        return;
    }

    if (event->x() < 10 && event->y() < 10)
        setCursor(Qt::SizeFDiagCursor);
    else if (event->x() < 10)
        setCursor(Qt::SizeHorCursor);
    else if (event->y() < 10)
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);

    if (pressed && drag) {
        setX(event->globalX() - click_x);
        setY(event->globalY() - click_y);
    }

    if (pressed && resize_w) {
        qDebug() << width() - (event->x() - click_x);
        //if (width() >= minimumWidth()) {
            setWidth(width() - (event->x() - click_x));
            setX(event->globalX() - click_x);
        //}
    }

    if (pressed && resize_h) {
        setHeight(height() - (event->y() - click_y));
        setY(event->globalY() - click_y);
    }

    QQuickWindow::mouseMoveEvent(event);
}


void PhoenixWindow::mouseDoubleClickEvent(QMouseEvent *event)
{

    if (!frameless()) {
        QQuickWindow::mouseDoubleClickEvent(event);
        return;
    }

    if (visibility() == QWindow::Windowed)
        setVisibility(QWindow::FullScreen);
    else
        setVisibility(QWindow::Windowed);
    QQuickWindow::mouseDoubleClickEvent(event);
}
