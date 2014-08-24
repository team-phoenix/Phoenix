
#include <QKeySequence>
#include "keyboard.h"
#include "logging.h"


Keyboard::Keyboard(InputDeviceMapping *mapping) : InputDevice(mapping)
{
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
            {"driver", "qt_keyboard"}
        }
    };
}

void Keyboard::processKeyEvent(QKeyEvent *event)
{
    bool is_pressed = (event->type() == QEvent::KeyPress) ? true : false;
    emit inputEventReceived(event->key(), is_pressed);

    auto retro_id = m_mapping->getMapping(event->key());
    if (retro_id != (unsigned)~0)
        setState(retro_id, is_pressed);
}

int32_t Keyboard::Mapping::eventFromString(QString evname)
{
    auto kseq = QKeySequence::fromString(evname);
    if (kseq.isEmpty() || kseq.count() != 1)
        return -1;
    return kseq[0];
}
