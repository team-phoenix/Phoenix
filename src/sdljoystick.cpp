#include "sdljoystick.h"
#include <QDebug>
#include <QProcess>

#define JOYSTICK_DEADZONE_LOW 2000
#define JOYSTICK_DEADZONE_HIGH 50000

SDLJoystick::SDLJoystick(QObject *parent)
    : QObject(parent) {

    qCDebug(phxInput) << this << "SDLJoystick";

}

SDLJoystick::~SDLJoystick() {
    qCDebug(phxInput) << this << "~SDLJoystick";
    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0)
        SDL_Quit();
}

/* public slots */

void SDLJoystick::setDeadZone(Uint16 &axis_value) {

    if (axis_value > JOYSTICK_DEADZONE_HIGH || axis_value < JOYSTICK_DEADZONE_LOW)
        axis_value = 0;

}

QList<InputDevice *> SDLJoystick::onScan() {

    qDeleteAll(joys.begin(), joys.end());
    joys.clear();
    qCDebug(phxInput) << "scanning";
    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
        qCDebug(phxInput) << "SDL_Quit";
        SDL_Quit();
    }

    if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
        qCCritical(phxInput) << "SDL_INIT_JOYSTICK failed";
        //return;
    }


    int count = SDL_NumJoysticks();
    Joystick *input_device;
    for (int i = 0; i < count; ++i) {
        input_device = new Joystick();

        input_device->index = i;

        input_device->name = SDL_JoystickNameForIndex(i);

        input_device->joystick = SDL_JoystickOpen(i);
        if (!input_device->joystick) {
            qCCritical(phxInput) << "SDL_JoystickOpen failed, index:" << i;
            joys.append(input_device);
            return joys;
        }

        //quint8 k;
        if (input_device->type == RETRO_DEVICE_ANALOG)
            input_device->count = SDL_JoystickNumAxes(input_device->joystick);
        else if (input_device->type == RETRO_DEVICE_JOYPAD)
            input_device->count = SDL_JoystickNumButtons(input_device->joystick);


        //int button_count = j->SDL_JoystickNumButtons(j->joystick);
        //for (k = 0; k < button_count; ++k)
        //    j->buttons[k] = 0;

        //int hat_count = j->SDL_JoystickNumHats(j->joystick);
        //for (k = 0; k < hat_count; ++k)
        //    j->hats[k] = 0;

        joys.append(input_device);

        qDebug("idx: %d, name: %s (buttons: %d)",
               input_device->index,
               input_device->name.toUtf8().data(),
               input_device->count);
    }

    return joys;

}

void SDLJoystick::convertToRetroID(unsigned &id, int button) {
    switch (button) {
        case 0:
            id = RETRO_DEVICE_ID_JOYPAD_UP;
            break;
        case 1:
            id = RETRO_DEVICE_ID_JOYPAD_DOWN;
            break;
        case 2:
            id = RETRO_DEVICE_ID_JOYPAD_LEFT;
            break;
        case 3:
            id = RETRO_DEVICE_ID_JOYPAD_RIGHT;
            break;
        case 4:
            id = RETRO_DEVICE_ID_JOYPAD_START;
            break;
        case 5:
            id = RETRO_DEVICE_ID_JOYPAD_SELECT;
            break;
        case 6:
            id = RETRO_DEVICE_ID_JOYPAD_L3;
            break;
        case 7:
            id = RETRO_DEVICE_ID_JOYPAD_R3;
            break;
        case 8:
            id = RETRO_DEVICE_ID_JOYPAD_L;
            break;
        case 9:
            id = RETRO_DEVICE_ID_JOYPAD_R;
            break;
        case 10:
            id = RETRO_DEVICE_ID_JOYPAD_B;
            break;
        case 11:
            id = RETRO_DEVICE_ID_JOYPAD_A;
            break;
        case 12:
            id = RETRO_DEVICE_ID_JOYPAD_Y;
            break;
        case 13:
            id = RETRO_DEVICE_ID_JOYPAD_X;
            break;
        case 14:
            // id = it's the home button on xbox controller
            break;

        case 15:
            break;
        default:
            qCDebug(phxInput) << "Input was not handled";
            break;
    }
}

void SDLJoystick::onProcessEvent(QList<InputDevice *> &devices) {

    Joystick *input_device = nullptr;
    unsigned id = 16;
    //unsigned device = RETRO_DEVICE_JOYPAD;
    for (int j = 0; j < devices.size(); ++j) {

        int i;
        input_device = static_cast<Joystick *>(devices.at(j));

        SDL_JoystickUpdate();

        // Currently left commented because no currently working core uses analog controls.

        /*for (i = 0; i < joy->numAxes; ++i) {

            Sint16 new_axis_motion = SDL_JoystickGetAxis(joy->joy, i);
            joy->axes[i] = new_axis_motion;
            device = RETRO_DEVICE_ANALOG;

            switch(i) {
                case 0:
                    id = RETRO_DEVICE_ID_ANALOG_X;
                    index = RETRO_DEVICE_INDEX_ANALOG_LEFT;

                    setDeadZone(new_axis_motion);

                    break;
                case 1:
                    id = RETRO_DEVICE_ID_ANALOG_Y;
                    index = RETRO_DEVICE_INDEX_ANALOG_LEFT;

                    setDeadZone(new_axis_motion);

                    break;
                case 2:
                    id = RETRO_DEVICE_ID_ANALOG_X;
                    index = RETRO_DEVICE_INDEX_ANALOG_RIGHT;

                    setDeadZone(new_axis_motion);

                    break;
                case 3:
                    id = RETRO_DEVICE_ID_ANALOG_Y;
                    index = RETRO_DEVICE_INDEX_ANALOG_RIGHT;

                    setDeadZone(new_axis_motion);

                    break;
                case 4:
                    id = RETRO_DEVICE_ID_JOYPAD_L2;
                    device = RETRO_DEVICE_JOYPAD;
                    break;
                case 5:
                    id = RETRO_DEVICE_ID_JOYPAD_R2;
                    device = RETRO_DEVICE_JOYPAD;
                    break;
                default:
                    qCDebug(phxInput) << "not handled: " << i << "   ==>>  " << new_axis_motion;
                    break;
            }
        }*/


        for (i = 0; i < input_device->count; ++i) {
            Sint16 new_button_press = static_cast<Sint16>(SDL_JoystickGetButton(input_device->joystick, i));
            convertToRetroID(id, i);
            input_device->button_states[id] = new_button_press;
            //qCDebug(phxInput) << i << "  ==> " << new_button_press;
        }

        // Currently left commented because my controllers don't use hats.

        //for (i = 0; i < joy->numHats; ++i) {
            //joy->hats[i] = SDL_JoystickGetHat(joy->joy, i);
            //qCDebug(phxInput) << joy->hats[i];
        //}

    }

}
