#pragma once

#include <QObject>
#include <QDebug>

#include "gamepadstate.h"
#include "node.h"

#include "SDL.h"
#include "SDL_gamecontroller.h"
#include "SDL_haptic.h"
#include "SDL_joystick.h"

/*
 * SDLUnloader's sole job is to sit past any and all input consumers so it may recieve RemoveController commands and call
 * appropiate SDL functions to clean up. This will prevent the use of any freed handles by only freeing them once
 * all consumers have consumed it.
 */

class SDLUnloader : public Node {
        Q_OBJECT

    public:
        SDLUnloader();

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;
};
