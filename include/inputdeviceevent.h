
#ifndef INPUTDEVICEEVENT_H
#define INPUTDEVICEEVENT_H

#include <functional>

#include <QtGlobal>
#include <QHash>
#include <QDebug>


class InputDeviceEvent
{
public:
    virtual InputDeviceEvent *clone() const = 0;

    // compare two device event
    virtual bool operator==(const InputDeviceEvent &other) const = 0;

    // to use instances as key in a QMap / std::map
    virtual uint qHash(uint seed) const = 0;
    virtual std::size_t stdhash() const = 0;

    // wether the object contains a valid event
    virtual bool isValid() const = 0;

    // event type
    int type() const { return m_type; }

    // allow to explicitely convert an Event to a QString
    explicit operator QString() const { return this->toString(); }

protected:
    InputDeviceEvent(int _type = 0) : m_type(_type) {}
    virtual QString toString() const = 0;
    int m_type;
};

// for use as key in std containers
namespace std
{
    template <>
    struct hash<InputDeviceEvent *>
    {
        std::size_t operator()(const InputDeviceEvent *k) const
        {
            return k->stdhash();
        }
    };

    template <>
    struct equal_to<InputDeviceEvent *> {
        bool operator() (const InputDeviceEvent *lhs, const InputDeviceEvent *rhs) const
        {
            return *lhs == *rhs;
        }
    };
}

// specialization for Button events (key press, joystick button press)
class InputDeviceButtonEvent : public InputDeviceEvent
{
protected:
    using InputDeviceEvent::InputDeviceEvent;
};

// specialization for Analog events (joystick axis...)
class InputDeviceAnalogEvent : public InputDeviceEvent
{
protected:
    using InputDeviceEvent::InputDeviceEvent;
};


// hackish way to generate unique id for types
// http://codereview.stackexchange.com/questions/44936/unique-type-id-in-c/45079#45079
// TODO: maybe use std::type_index instead ?..
template<typename T>
struct input_event_types { static void id() { } };

template<typename T>
size_t input_event_type_id() { return reinterpret_cast<size_t>(&input_event_types<T>::id); }

// helpers for perferct forwarding
template <typename T, typename U>
using enable_if_same_t =
    typename std::enable_if<
        std::is_same<typename std::decay<T>::type, U>::value
    >::type;

template <typename T, typename U>
using enable_if_not_same_or_derived_t =
    typename std::enable_if<
        !std::is_same<typename std::decay<T>::type, U>::value
        && !std::is_base_of<U, typename std::decay<T>::type>::value
    >::type;

// allow to customize the default value for some TEvent types.
// E.g. some fucking SDL enums use a default (invalid) value of -1, so the
// default constructed value TEvent() would refer to a valid button/axis 0
template<typename T>
struct TEvent_default_t
{
    operator T() const { return T(); }
};

template<typename T>
bool operator==(const T &lhs, const TEvent_default_t<T> &rhs)
{
    return lhs == static_cast<T>(rhs);
}
template <typename T>
bool operator!=(const T &lhs, const TEvent_default_t<T> &rhs)
{
    return lhs != static_cast<T>(rhs);
}

// templated implementation of the InputDeviceEvent interface
// should be easily usable for sdl joysticks and keyboard...
//
// Tbase: Base class to inherit (eg, AnalogEvent or ButtonEvent)
// T: Type of the inherited class itself
// TEvent: Type of the underlying event (sdl event, qt event...)
template <class TBase, class T, typename TEvent>
class InputDeviceEventHelper : public TBase
{
public:
    // perfect-forwarding-like, but only accepts U == TEvent as to not
    // end up doing weird stuff like shadowing copy / move operators
    template <typename U, typename Disable = enable_if_same_t<U, TEvent>>
    InputDeviceEventHelper(U &&event)
        : TBase(input_event_type_id<T>()),
          m_event(std::forward<U>(event))
    {
    }

    // default constructed event is "invalid"
    InputDeviceEventHelper() : TBase(input_event_type_id<T>()), m_event(default_t) {}

    virtual InputDeviceEvent *clone() const override
    {
        return new T(static_cast<const T &>(*this));
    }

    virtual bool operator==(const InputDeviceEvent &other) const override
    {
        return (this->m_type == other.type()
                && m_event == static_cast<const InputDeviceEventHelper &>(other).event());
    }

    virtual uint qHash(uint seed) const override
    {
        return ::qHash(this->m_type, seed) ^ ::qHash<TEvent>(m_event, seed);
    }

    virtual std::size_t stdhash() const override
    {
        return std::hash<int>()(this->m_type) ^ std::hash<TEvent>()(m_event);
    }

    virtual bool isValid() const override
    {
        return m_event != default_t;
    }

    const TEvent &event() const { return m_event; }

protected:
    template <typename U>
    void setEvent(U &&event) { m_event = std::forward<U>(event); }

    // typedef our own type here so derived class can refer to it easily
    // without having to type the whole templated class name again
    using HelperType = InputDeviceEventHelper<TBase, T, TEvent>;

private:
    TEvent m_event;
    TEvent_default_t<TEvent> default_t;
};


#endif
