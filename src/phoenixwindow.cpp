#include <phoenixwindow.h>
#include <QDebug>

PhoenixWindow::PhoenixWindow()
{
    m_surface_format = requestedFormat();
}

PhoenixWindow::~PhoenixWindow()
{

}

void PhoenixWindow::setWindowScreen(QScreen *screen)
{
    setScreen(screen);
}

void PhoenixWindow::setWinSwapBehavior(QSurfaceFormat::SwapBehavior behavior) {
    m_surface_format.setSwapBehavior(behavior);
}

void PhoenixWindow::setWinSwapInterval(int interval)
{
    m_surface_format.setSwapInterval(interval);
}

void PhoenixWindow::setWinFormat()
{
    setFormat(m_surface_format);
}
