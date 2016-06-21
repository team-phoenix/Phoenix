#pragma once

#include <QObject>
#include <QPointF>

struct MouseState {
    MouseState() = default;
    ~MouseState() = default;

    QPointF position;
    Qt::MouseButtons buttons;
};
