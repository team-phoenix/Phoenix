#include <phoenixwindow.h>

PhoenixWindow::PhoenixWindow()
{
    m_surface_format = requestedFormat();
    // Fix a bug that appears when you leave the software with alt + f4 when it is in fullscreen. This blocks the software fullscreen
    this->show();
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
