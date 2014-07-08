#ifndef JOYINFO_H
#define JOYINFO_H

#include <QObject>
#include <QMap>
#include <SDL2/SDL.h>

class Joystick
{
public:
    int index;
    QString name;
    int numAxes;
    int numButtons;
    int numHats;
    SDL_Joystick *joy;
    QMap<quint8, Sint16> axes;
    QMap<quint8, Uint8> buttons;
    QMap<quint8, Uint8> hats;

    Joystick();
    ~Joystick();
};

#endif // JOYINFO_H
