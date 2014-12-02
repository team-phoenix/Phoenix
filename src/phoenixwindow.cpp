#include <phoenixwindow.h>

#include <QLibrary>
#include <QFile>

#ifdef Q_OS_WIN32
    #include <windowsx.h>
    #include <windows.h>
    #include <QApplication>
    #include <QDesktopWidget>
    #include <QPainter>
    #include <QPixmap>
    #include <qt_windows.h>
    #include <QtWinExtras/qwinfunctions.h>
#endif

PhoenixWindow::PhoenixWindow()
{
    m_surface_format = requestedFormat();
    m_frameless = false;
    mouse_pressed = false;
    // Fix a bug that appears when you leave the software with alt + f4 when it is in fullscreen. This blocks the software fullscreen
    //this->show();

}

PhoenixWindow::~PhoenixWindow()
{

}

bool PhoenixWindow::loadCore(QString core_path)
{
    QLibrary libretro_core(core_path);
    bool result = libretro_core.load();
    libretro_core.unload();
    return result;
}

bool PhoenixWindow::loadGame(QString game_path)
{
    QFile in_file(game_path);
    bool result = in_file.open(QIODevice::ReadOnly);

    in_file.close();
    return result;
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
        setFlags(Qt::FramelessWindowHint | Qt::Window);
    emit framelessChanged();
}

/*void PhoenixWindow::mousePressEvent(QMouseEvent *event)
{

    //qCDebug(phxLibrary) << mouse_pressed;

    if (!this->frameless() || event->button() != Qt::LeftButton) {
        QQuickWindow::mousePressEvent(event);
        return;
    }

    else {
        mouse_pressed = true;
    }

    QQuickWindow::mousePressEvent(event);


}

void PhoenixWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (!this->frameless() || event->button() != Qt::LeftButton) {
        QQuickWindow::mousePressEvent(event);
        return;
    }

    else {
        mouse_pressed = false;
        qCDebug(phxLibrary) << mouse_pressed;

    }

    QQuickWindow::mousePressEvent(event);

}*/

#ifdef Q_OS_WIN32
void PhoenixWindow::paintPicture(MSG *pMessage, long *result)
{
    RECT winRect1;
        RECT winRectCl;
        ::GetWindowRect(windowHandle, &winRect1);
        ::GetClientRect(windowHandle, &winRectCl);
        HDC hDeviceContext = ::GetWindowDC(windowHandle);
        LONG lStyle = ::GetWindowLong(windowHandle, GWL_STYLE);

        POINT ptTopLeft = {winRectCl.left, winRectCl.top};
        POINT ptBottomRight = {winRectCl.right, winRectCl.bottom};

        ::ClientToScreen(windowHandle, &ptTopLeft);
        ::ClientToScreen(windowHandle, &ptBottomRight);
        winRectCl.left = ptTopLeft.x - winRect1.left;
        winRectCl.top = ptTopLeft.y - winRect1.top;
        winRectCl.right = ptBottomRight.x - winRect1.left;
        winRectCl.bottom = ptBottomRight.y - winRect1.top;

        winRect1.right = winRect1.right - winRect1.left;
        winRect1.bottom = winRect1.bottom - winRect1.top;
        winRect1.top = 0;
        winRect1.left = 0;

        HRGN hRgnOuter = ::CreateRectRgn(winRect1.left, winRect1.top, winRect1.right, winRect1.bottom);
        HRGN hRgnInner = ::CreateRectRgn(winRectCl.left, winRectCl.top, winRectCl.right, winRectCl.bottom);
        HRGN hRgnCombine = ::CreateRectRgn(winRect1.left, winRect1.top, winRect1.right, winRect1.bottom);

        ::CombineRgn(hRgnCombine, hRgnOuter, hRgnInner, RGN_DIFF);
        ::SelectClipRgn(hDeviceContext, hRgnCombine);

        QPixmap pix(winRect1.right, winRect1.bottom);
        QPainter paint(&pix);
        QRect rc(0,0,winRect1.right, winRect1.bottom);

        paint.fillRect(rc, QColor(28,28,28));
        QLinearGradient grad(0,0,0,40);
        grad.setColorAt(0, QColor(255,255,255,180));
        grad.setColorAt(0.33, QColor(255,255,255,80));
        grad.setColorAt(0.33, QColor(255,255,255,0));
        grad.setColorAt(1, QColor(255,255,255,0));
        paint.fillRect(QRect(0,0,winRect1.right, 40), grad);

        HBITMAP hBmp = QtWin::toHBITMAP(pix);

        HDC hDC = ::CreateCompatibleDC(hDeviceContext);
        ::SelectObject(hDC, hBmp);
        ::BitBlt(hDeviceContext, winRect1.left, winRect1.top, winRect1.right, winRect1.bottom, hDC, 0, 0, SRCCOPY);
        ::DeleteDC(hDC);
        ::DeleteObject(hBmp);

        ::DeleteObject(hRgnOuter);
        ::DeleteObject(hRgnInner);
        ::DeleteObject(hRgnCombine);
        ::ReleaseDC(windowHandle, hDeviceContext);
        *result = 0;
}
#endif

bool PhoenixWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
/*
#ifdef Q_OS_WIN32
    if (!frameless()) {
        if (eventType == "windows_generic_MSG") {
            MSG *win_msg = (MSG *)message;
            if (!windowHandle)
                windowHandle = win_msg->hwnd;
            switch(win_msg->message) {
                case WM_CLOSE: {
                    return close();
                    break;
                }
                case WM_SETFOCUS: {
                    qCDebug(phxLibrary) << "Got focus";
                    paintPicture(win_msg, result);
                    break;
                }

                case WM_KILLFOCUS: {
                    qCDebug(phxLibrary) << "Lost Focus";
                    paintPicture(win_msg, result);
                    break;
                }

                case WM_ACTIVATE: {

                    if (LOWORD(win_msg->wParam) == WA_ACTIVE)
                        qCDebug(phxLibrary) << "Activated window";
                    else
                        qCDebug(phxLibrary) << "I am inactive window";
                    break;
                }

                case WM_NCCALCSIZE: {
                    if (!frameless())
                        return 0;
                    break;
                }

                case WM_NCPAINT: {
                    paintPicture(win_msg, result);
                    return true;
                }

                /*case WM_GETMINMAXINFO: {
                    QRect maxRect = QApplication::desktop()->availableGeometry();
                    QSize szMin = this->minimumSize();
                    QSize szMax = this->maximumSize();
                    PMINMAXINFO lpMMI = (PMINMAXINFO)win_msg->lParam;
                    lpMMI->ptMaxSize.x = maxRect.width();
                    lpMMI->ptMaxSize.y = maxRect.height();
                    lpMMI->ptMaxPosition.x = 0;
                    lpMMI->ptMaxPosition.y = 0;
                    lpMMI->ptMinTrackSize.x = szMin.width();
                    lpMMI->ptMinTrackSize.y = szMin.height();
                    lpMMI->ptMaxTrackSize.x = szMax.width();
                    lpMMI->ptMaxTrackSize.y = szMax.height();
                    *result = 0;
                    return true;
                }*/

                /*case WM_NCHITTEST: {
                    const LONG borderWidth = 8; //in pixels
                    RECT winrect;
                    GetWindowRect(windowHandle, &winrect);
                    long x = GET_X_LPARAM(win_msg->lParam);
                    long y = GET_Y_LPARAM(win_msg->lParam);


                    //bottom left corner
                    if ( x >= winrect.left && x < winrect.left + borderWidth &&
                      y < winrect.bottom && y >= winrect.bottom - borderWidth )
                    {
                      *result =  HTBOTTOMLEFT;
                    }
                    //bottom right corner
                    if ( x < winrect.right && x >= winrect.right - borderWidth &&
                      y < winrect.bottom && y >= winrect.bottom - borderWidth )
                    {
                      *result = HTBOTTOMRIGHT;
                    }
                    //top left corner
                    if ( x >= winrect.left && x < winrect.left + borderWidth &&
                      y >= winrect.top && y < winrect.top + borderWidth )
                    {
                      *result = HTTOPLEFT;
                    }
                    //top right corner
                    if ( x < winrect.right && x >= winrect.right - borderWidth &&
                      y >= winrect.top && y < winrect.top + borderWidth )
                    {
                      *result = HTTOPRIGHT;
                    }
                    //left border
                    if ( x >= winrect.left && x < winrect.left + borderWidth )
                    {
                      *result = HTLEFT;
                    }
                    //right border
                    if ( x < winrect.right && x >= winrect.right - borderWidth )
                    {
                      *result = HTRIGHT;
                    }
                    //bottom border
                    if ( y < winrect.bottom && y >= winrect.bottom - borderWidth )
                    {
                      *result = HTBOTTOM;
                    }
                    //top border
                    if ( y >= winrect.top && y < winrect.top + borderWidth )
                    {
                      *result = HTTOP;
                    }

                    if (*result == 0) {
                        *result = HTCAPTION;
                    }
                    return true;
                }

                default:
                    break;
            }
        }
    }
#endif
*/

    return QQuickWindow::nativeEvent(eventType, message, result);

}


