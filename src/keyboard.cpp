
#include "keyboard.h"
#include "logging.h"


Keyboard::Keyboard()
{
    setType(RETRO_DEVICE_JOYPAD);
    setDeviceName("Keyboard (Qt KeyEvent)");
}

Keyboard::~Keyboard()
{

}

QVariantList Keyboard::enumerateDevices()
{
    return {
        QVariantMap {
            {"text", "Keyboard"}, // for QML model
            {"name", "Keyboard"},
            {"class", "Keyboard"}
        }
    };
}

void Keyboard::processKeyEvent(QKeyEvent *event)
{
    int id;

    switch (event->key()) {
        case Qt::Key_Up:
            id = RETRO_DEVICE_ID_JOYPAD_UP;
            break;
        case Qt::Key_Down:
            id = RETRO_DEVICE_ID_JOYPAD_DOWN;
            break;
        case Qt::Key_Left:
            id = RETRO_DEVICE_ID_JOYPAD_LEFT;
            break;
        case Qt::Key_Right:
            id = RETRO_DEVICE_ID_JOYPAD_RIGHT;
            break;
        case Qt::Key_Return:
            id = RETRO_DEVICE_ID_JOYPAD_START;
            break;
        case Qt::Key_Backspace:
            id = RETRO_DEVICE_ID_JOYPAD_SELECT;
            break;
        case Qt::Key_Shift:
            id = RETRO_DEVICE_ID_JOYPAD_L;
            break;
        case Qt::Key_Control:
            id = RETRO_DEVICE_ID_JOYPAD_R;
            break;
        case Qt::Key_A:
            id = RETRO_DEVICE_ID_JOYPAD_X;
            break;
        case Qt::Key_Z:
            id = RETRO_DEVICE_ID_JOYPAD_A;
            break;
        case Qt::Key_X:
            id = RETRO_DEVICE_ID_JOYPAD_B;
            break;
        case Qt::Key_S:
            id = RETRO_DEVICE_ID_JOYPAD_Y;
            break;
        default:
            break;
    }

    bool is_pressed = (event->type() == QEvent::KeyPress) ? true : false;
    setState(id, is_pressed);
}
