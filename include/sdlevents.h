
#ifndef SDLEVENTS_H
#define SDLEVENTS_H

#include <functional>
#include <QObject>
#include <QTimer>
#include <QList>
#include <SDL2/SDL.h>

#include "inputdevice.h"


class SDLEvents : public QObject
{
    Q_OBJECT
public:
    SDLEvents();
    virtual ~SDLEvents();

    typedef std::function<void(const SDL_Event *)> EventCallback;

    void registerCallback(EventCallback *cb) {
        event_callbacks.append(cb);
    }

    void removeCallback(EventCallback *cb) {
        event_callbacks.removeOne(cb);
    }

protected slots:
    void pollSDL();

private:
    QTimer polltimer;

    // buffer used to temporarily store events we got from SDL
    SDL_Event *event_list;

    QList<EventCallback*> event_callbacks;
};

#endif
