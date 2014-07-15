#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include "libretro.h"
#include "logging.h"

#include <QMap>
#include <QString>
#include <SDL2/SDL.h>

class InputDevice {

public:
    InputDevice();

    ~InputDevice();

    void setType(unsigned new_type);
    void setCount(int new_count);
    void setName(const char *new_name);
    void setIndex(int new_index);

    QMap<unsigned, Sint16> button_states;

    unsigned type;

    int index;

    int count;

    QString name;


};

#endif // INPUTDEVICE_H
