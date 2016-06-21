#pragma once

#include <QObject>

#include "node.h"

/*
 * GlobalGamepad is a Node designed to present Phoenix's QML-based UI with an OR of all controller states (with the exception
 * of the keyboard). It is meant to be used to control the UI with whatever game controller is connected to the system.
 */

class GlobalGamepad : public Node {
        Q_OBJECT

    public:
        explicit GlobalGamepad( Node *parent = nullptr );

    signals:

    public slots:
};
