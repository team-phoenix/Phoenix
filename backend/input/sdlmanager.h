#pragma once

#include <QObject>
#include <QMutex>
#include <QHash>

#include "gamepadstate.h"
#include "node.h"

#include "SDL.h"
#include "SDL_gamecontroller.h"
#include "SDL_haptic.h"
#include "SDL_joystick.h"

/*
 * SDLManager is a Node whose job is to produce input data from physical controllers connected to the system running
 * Phoenix. The polling of controllers for their existence and input data is driven by heartbeats from this node's parent.
 */

class SDLManager : public Node {
        Q_OBJECT

    public:
        explicit SDLManager( Node *parent = nullptr );

    signals:

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;

    private:
        QMutex mutex;

        // A list of stored button states, indexed by instanceID
        QHash<int, GamepadState> gamepads;

        // A circular buffer that holds gamepad state updates
        // A value of 100 should be sufficient for most purposes
        GamepadState gamepadBuffer[ 100 ];
        int gamepadBufferIndex{ 0 };

        QString userDataPath;
};
