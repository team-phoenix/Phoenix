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
    void setWinSwapInterval(int interval);
    void setWinSwapBehavior(QSurfaceFormat::SwapBehavior behavior);

private slots:

signals:

};

#endif // PHOENIXWINDOW_H
