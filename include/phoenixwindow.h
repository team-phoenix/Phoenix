#ifndef PHOENIXWINDOW_H
#define PHOENIXWINDOW_H

#include <QQuickWindow>
#include <QSurfaceFormat>

#include "logging.h"

/* The PhoenixWindow class subclass of the QQuickWindow class.
 *
 * Under normal cases, a developer would just have the
 * root window of their QML application be an ApplicationWindow,
 * or even just a standard Window.
 *
 * These methods are fine for normal cases, but the default QQuickWindow class
 * does not provide slots for changing things like the swap behavior of the window.
 *
 * If you want to remove native system border around Phoenix, that operation will be done through
 * this class.
 *
 * This class is exposed to QML and is the root most window of Phoenix.
 */

class PhoenixWindow : public QQuickWindow {
        Q_OBJECT
        Q_PROPERTY( int swapInterval READ swapInterval WRITE setWinSwapInterval NOTIFY swapIntervalChanged )
        Q_PROPERTY( int swapBehavior READ swapBehavior WRITE setWinSwapBehavior NOTIFY swapBehaviorChanged )
        Q_PROPERTY( bool frameless READ frameless WRITE setFrameless NOTIFY framelessChanged )

    public:
        PhoenixWindow();
        ~PhoenixWindow();
        void setWindowScreen( QScreen *screen );
        void setWinFormat();

        QScreen *window_screen() {
            return screen();
        }

        int swapInterval() const {
            return m_swap_interval;
        }

        int swapBehavior() const {
            return m_swap_behavior;
        }

        bool frameless() const {
            return m_frameless;
        }

    public slots:

        // After window->show(), these functions do not change any behavior.
        // These functions need to be set before the program starts and the program
        // needs to be reset completely for the changes to take effect.
        // [1]
        void setWinSwapInterval( int interval );
        void setWinSwapBehavior( int behavior );
        // ~[1]

        void setFrameless( bool frameless );

    private slots:

    signals:
        void swapIntervalChanged( int interval );
        void swapBehaviorChanged( int behavior );
        void framelessChanged();

    private:
        QSurfaceFormat m_surface_format;
        int m_swap_interval;
        int m_swap_behavior;
        bool mouse_pressed;
        bool m_frameless;

#ifdef Q_OS_WIN32
        HWND windowHandle = nullptr;
        void paintPicture( MSG *pMessage, long *result );
#endif


        //void mousePressEvent(QMouseEvent *event);
        // void mouseReleaseEvent(QMouseEvent *event);
        bool nativeEvent( const QByteArray &eventType, void *message, long *result );
};

#endif // PHOENIXWINDOW_H
