#ifndef PHOENIXWINDOW_H
#define PHOENIXWINDOW_H

#include <QQuickWindow>
#include <QSurfaceFormat>

#include "logging.h"

class PhoenixWindow : public QQuickWindow {
    Q_OBJECT
    Q_PROPERTY(int swapInterval READ swapInterval WRITE setWinSwapInterval NOTIFY swapIntervalChanged)
    Q_PROPERTY(int swapBehavior READ swapBehavior WRITE setWinSwapBehavior NOTIFY swapBehaviorChanged)
    Q_PROPERTY(bool frameless READ frameless WRITE setFrameless NOTIFY framelessChanged)

public:
    PhoenixWindow();
    ~PhoenixWindow();
    void setWindowScreen(QScreen *screen);

    QScreen *window_screen() {
        return screen();
    }

    int swapInterval() const
    {
        return m_swap_interval;
    }

    int swapBehavior() const
    {
        return m_swap_behavior;
    }

    void setWinFormat();

    bool frameless() const
    {
        return m_frameless;
    }

public slots:

    // After window->show(), these functions do not change any behavior.
    // These functions need to be set before the program starts and the program
    // needs to be reset completely for the changes to take effect.
    // [1]
    void setWinSwapInterval(int interval);
    void setWinSwapBehavior(int behavior);
    // ~[1]

    void setFrameless(bool frameless);

    static bool loadCore(QString core_path);
    static bool loadGame(QString game_path);

private slots:

signals:
    void swapIntervalChanged(int);
    void swapBehaviorChanged(int);
    void framelessChanged();

private:
    QSurfaceFormat m_surface_format;
    int m_swap_interval;
    int m_swap_behavior;
    bool mouse_pressed;
    bool m_frameless;

#ifdef Q_OS_WIN32
    HWND windowHandle = nullptr;
    void paintPicture(MSG *pMessage, long *result);
#endif


    //void mousePressEvent(QMouseEvent *event);
   // void mouseReleaseEvent(QMouseEvent *event);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
};

#endif // PHOENIXWINDOW_H
