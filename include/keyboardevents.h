
#ifndef KEYBOARDEVENTS_H
#define KEYBOARDEVENTS_H

#include <utility>
#include <Qt>

// must be defined before #include <QHash>
typedef std::pair<Qt::Key, Qt::KeyboardModifiers> QtKeyboardKeyEvent;
inline uint qHash(const QtKeyboardKeyEvent &kev, uint seed = 0);

#include <QKeyEvent>
#include <SDL2/SDL.h>

#include "inputdeviceevent.h"


inline uint qHash(const QtKeyboardKeyEvent &kev, uint seed)
{
    return (qHash<int>(static_cast<int>(kev.first), seed) ^
            qHash<unsigned>(static_cast<unsigned>(kev.second), seed));
}

namespace std
{
    template <>
    struct hash<QtKeyboardKeyEvent>
    {
        std::size_t operator()(const QtKeyboardKeyEvent &kev) const
        {
            return (hash<int>()(static_cast<int>(kev.first)) ^
                    hash<unsigned>()(static_cast<unsigned>(kev.second)));
        }
    };
}

class KeyboardKeyEvent : public InputDeviceEventHelper<InputDeviceButtonEvent, KeyboardKeyEvent, QtKeyboardKeyEvent>
{
    using super = HelperType;

public:
    using super::super;

    static KeyboardKeyEvent fromKeyEvent(const QKeyEvent *event)
    {
        const Qt::Key key = static_cast<Qt::Key>(event->key());
        Qt::KeyboardModifiers mod = event->modifiers();
        if (key == Qt::Key_Control || key == Qt::Key_Shift
              || key == Qt::Key_Alt || key == Qt::Key_Meta) {
            mod = Qt::KeyboardModifiers(); // no modifiers
        }
        return KeyboardKeyEvent(QtKeyboardKeyEvent(key, mod));
    }

    static KeyboardKeyEvent fromString(const QString &str)
    {
        auto kseq = QKeySequence::fromString(str);
        if (kseq.count() == 1 && kseq[0] != Qt::Key_unknown) {
            auto key = static_cast<Qt::Key>(kseq[0] & ~Qt::KeyboardModifierMask);
            auto mod = static_cast<Qt::KeyboardModifier>(kseq[0] & Qt::KeyboardModifierMask);
            return KeyboardKeyEvent(QtKeyboardKeyEvent(key, mod));
        }

        // hack for modifiers keys (ctrl..)
        kseq = QKeySequence::fromString(str + "+A");
        if (kseq.count() == 1) {
            int keys = 0;
            auto mod = static_cast<Qt::KeyboardModifier>(kseq[0] & Qt::KeyboardModifierMask);

            if (mod & Qt::ShiftModifier)
                keys |= Qt::Key_Shift;

            if (mod & Qt::ControlModifier)
                keys |= Qt::Key_Control;

            if (mod & Qt::MetaModifier)
                keys |= Qt::Key_Meta;

            if (mod & Qt::AltModifier)
                keys |= Qt::Key_Alt;

            return KeyboardKeyEvent(QtKeyboardKeyEvent(static_cast<Qt::Key>(keys),
                                                       Qt::KeyboardModifiers()));
        }

        return KeyboardKeyEvent();
    }

protected:
    virtual QString toString() const override
    {
        const Qt::Key key = event().first;
        const Qt::KeyboardModifiers &mod = event().second;
        if (key == Qt::Key_Control || key == Qt::Key_Shift
              || key == Qt::Key_Alt || key == Qt::Key_Meta) {
            return QKeySequence(key).toString();
        } else {
            return QKeySequence(key + mod).toString();
        }
    }
};

#endif
