
#include <QGuiApplication>
#include <QWindow>
#include <QKeySequence>
#include "keyboard.h"
#include "logging.h"


Keyboard::Keyboard(InputDeviceMapping *mapping) : InputDevice(mapping)
{
    setDeviceName("Keyboard (Qt KeyEvent)");
    topLevelWindow = QGuiApplication::topLevelWindows()[0];
    topLevelWindow->installEventFilter(this);
}

Keyboard::~Keyboard()
{
    topLevelWindow->removeEventFilter(this);
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

bool Keyboard::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    auto type = event->type();
    if (type == QEvent::KeyPress || type == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        processKeyEvent(keyEvent);
        return true;
    }
    return false;
}

inline void Keyboard::processKeyEvent(QKeyEvent *event)
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
