#ifndef PHOENIXWINDOW_H
#define PHOENIXWINDOW_H

#include <QQuickWindow>
#include <QSurfaceFormat>

class PhoenixWindow : public QQuickWindow {
    Q_OBJECT
public:
    PhoenixWindow();
    ~PhoenixWindow();
    void setWindowScreen(QScreen *screen);

    QScreen *window_screen() {
        return screen();
    }

    void setWinFormat();

private:
    QSurfaceFormat m_surface_format;

public slots:

    // After window->show(), these functions do not change any behavior.
    // These functions need to be set before the program starts and the program
    // needs to be reset completely for the changes to take effect.
    // [1]
    void setWinSwapInterval(int interval);
    void setWinSwapBehavior(QSurfaceFormat::SwapBehavior behavior);
    // ~[1]

private slots:

signals:

};

#endif // PHOENIXWINDOW_H
