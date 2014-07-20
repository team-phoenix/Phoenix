
#include <QFile>

#include "sdlevents.h"
#include "logging.h"

#ifdef Q_OS_LINUX
#include <signal.h>
#endif


SDLEvents::SDLEvents()
{
    // Initialize the GameController database with the most recent file
    // from https://github.com/gabomdq/SDL_GameControllerDB
    // TODO: Instead of storing the file as a ressource, have it in some
    // directory so the user can modify it if needed..
    QFile f(":/assets/gamecontrollerdb.txt");
    f.open(QIODevice::ReadOnly);
    SDL_SetHint(SDL_HINT_GAMECONTROLLERCONFIG, f.readAll().constData());

#ifdef Q_OS_LINUX
    struct sigaction action;
    sigaction(SIGINT, NULL, &action);
#endif

    // this will also implicitly initialize the event loop
    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
        qFatal("Fatal: Unable to initialize SDL2: %s", SDL_GetError());
    }

#ifdef Q_OS_LINUX
    // restore SIGINT interrupt handler.
    // this prevents SDL from handling CTRL-C interrupts on linux
    sigaction(SIGINT, &action, NULL);
#endif

    connect(&polltimer, SIGNAL(timeout()), this, SLOT(pollSDL()));
    event_list = new SDL_Event[10]();
    polltimer.start(10);
}

SDLEvents::~SDLEvents()
{
    if (SDL_WasInit(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
        SDL_Quit();

    delete[] event_list;
}

void SDLEvents::pollSDL()
{
    // consume moar events
    SDL_PumpEvents();
    int ret = SDL_PeepEvents(event_list, 10, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
    if (ret < 0)
        qCCritical(phxInput, "SDLEvents: unable to retrieve events: %s", SDL_GetError());
    
    SDL_Event *event = event_list;
    for (int i = 0; i < ret; i++) {
        foreach (auto cb, event_callbacks) {
            (*cb)(event);
        }
        event++;
    }
}

