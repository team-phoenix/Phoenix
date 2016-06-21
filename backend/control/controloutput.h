#pragma once

#include <QObject>

#include "node.h"

/*
 * ControlOutput is a Node that acknowledges state changes from its parent (a subclass of Core). In addition, it (TODO) exposes
 * properties to QML that help QML decide what buttons should be available to the user to click, such as pausable for
 * being able to click pause, etc.
 */

class ControlOutput : public Node {
        Q_OBJECT
        Q_PROPERTY( State state MEMBER state NOTIFY stateChanged )

    public:
        explicit ControlOutput( Node *parent = nullptr );

    signals:
        void paused();
        void stateChanged( State state );

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;

    private:
        State state;
};
