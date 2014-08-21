
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
            {"class", "Keyboard"}
        }
    };
}

void Keyboard::processKeyEvent(QKeyEvent *event)
{
    bool is_pressed = (event->type() == QEvent::KeyPress) ? true : false;
    auto retro_id = m_mapping->getMapping(event->key());
    if (retro_id != (unsigned)~0)
        setState(retro_id, is_pressed);
}

int32_t Keyboard::Mapping::eventFromString(QString evname)
{
    // TODO implement this using maybe QKeySequence
    auto kseq = QKeySequence::fromString(evname);
    if (kseq.isEmpty() || kseq.count() != 1)
        return -1;
    return kseq[0];
}
