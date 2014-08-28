
#include <functional>
#include <QMap>
#include <QPair>
#include "libretro.h"

#include "joystick.h"
#include "joystickevents.h"
#include "logging.h"


static SDLEvents sdl_events;


Joystick::Joystick(InputDeviceMapping *mapping) : InputDevice(mapping)
{
    device_attached = false;
    joystick = nullptr;
    controller = nullptr;
    m_mapping = reinterpret_cast<Mapping *>(InputDevice::m_mapping);
    Q_ASSERT(m_mapping != nullptr);

    callback = std::bind(&Joystick::handleSDLEvent, this, std::placeholders::_1);
    sdl_events.registerCallback(&callback);

    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (!m_mapping->matchJoystick(SDL_JoystickGetDeviceGUID(i)))
            continue;

        if (SDL_IsGameController(i)) {
            attachGameController(i);
            break;
        } else {
            attachJoystick(i);
            break;
        }
    }
}

Joystick::~Joystick()
{
    auto l = sdl_events.lockSDL();
    sdl_events.removeCallback(&callback);
    if (controller) {
        SDL_GameControllerClose(controller);
    } else if (joystick) {
        SDL_JoystickClose(joystick);
    }
    joystick = nullptr;
    controller = nullptr;
}

// static
QVariantList Joystick::enumerateDevices()
{
    QVariantList list;
    char guidbuf[128];
    int nconnected = SDL_NumJoysticks();

    if (!nconnected) {
        qCCritical(phxInput, "Unable to enumerate joysticks: %s", SDL_GetError());
    }

    for (int i = 0; i < nconnected; i++) {
        const char* jsname = SDL_JoystickNameForIndex(i);
        if (!jsname)
            continue;
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(i), guidbuf, 128);
        list.append(QVariantMap {
            { "text", QString(jsname) }, // for QML model
            { "name", QString(jsname) },
            { "driver", QString(SDL_IsGameController(i) ? "sdl_gamecontroller"
                                                        : "sdl_joystick") },
            { "guid", QString(guidbuf) }
        });
    }
    return list;
}

bool Joystick::attachJoystick(int which)
{
    Q_ASSERT(device_attached != true);
    joystick = SDL_JoystickOpen(which);
    if (joystick == nullptr) {
        qCWarning(phxInput, "Joystick: Unable to open sdl joystick: %s",
                            SDL_GetError());
        return false;
    }
    setDeviceName(SDL_JoystickName(joystick));
    device_attached = true;
    return true;
}

bool Joystick::attachGameController(int which)
{
    Q_ASSERT(device_attached != true);
    controller = SDL_GameControllerOpen(which);
    if (controller == nullptr) {
        qCWarning(phxInput, "Joystick: Unable to open sdl game controller: %s",
                            SDL_GetError());
        return false;
    }
    setDeviceName(SDL_GameControllerName(controller));
    device_attached = true;
    return true;
}

bool Joystick::deviceAdded(const SDL_Event *event)
{
    int which = event->type == SDL_CONTROLLERDEVICEADDED ? event->cdevice.which
                                                         : event->jdevice.which;
    if (!m_mapping->matchJoystick(SDL_JoystickGetDeviceGUID(which)))
        return false;

    if (event->type == SDL_CONTROLLERDEVICEADDED) {
        attachGameController(which);
    } else {
        if (SDL_IsGameController(which)) {
            // joystick is a supported game controller
            // let's wait for the CONTROLLERADDED event to add it
            return false;
        }
        attachJoystick(which);
    }
    return true;
}

bool Joystick::deviceRemoved(const SDL_Event *event)
{
    if (event->type == SDL_CONTROLLERDEVICEREMOVED
            && ControllerMatchEvent(event->cdevice)) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
        device_attached = false;
        qCDebug(phxInput) << "Controller removed";
        return true;
    } else if(JoystickMatchEvent(event->jdevice)) {
        SDL_JoystickClose(joystick);
        joystick = nullptr;
        device_attached = false;
        qCDebug(phxInput) << "Joystick removed";
        return true;
    }
    return false;
}

bool Joystick::controllerButtonChanged(const SDL_Event *event) {
    if (!ControllerMatchEvent(event->cbutton))
        return false;

    const SDL_ControllerButtonEvent *cbutton = &event->cbutton;
    auto ev = ControllerButtonEvent::fromSDLEvent(*cbutton);
    bool is_pressed = (cbutton->type == SDL_CONTROLLERBUTTONDOWN) ? true : false;

    emit inputEventReceived(&ev, is_pressed);
    auto retro_id = m_mapping->getMapping(&ev);
    if (retro_id != (unsigned)~0)
        setState(retro_id, is_pressed);

    return true;
}

bool Joystick::controllerAxisChanged(const SDL_Event *event) {
    if (!ControllerMatchEvent(event->caxis))
        return false;

    const SDL_ControllerAxisEvent *caxis = &event->caxis;
    auto ev = ControllerAxisEvent::fromSDLEvent(*caxis);
    emit inputEventReceived(&ev, caxis->value);

    // make analog stick emulate dpad
    if (m_mapping->deviceType() == RETRO_DEVICE_JOYPAD) {
        static int16_t threshold = 25000; // arbitrary. TODO: make it configurable
        static const QMap<SDL_GameControllerAxis, QPair<unsigned, unsigned>> mapping {
            { SDL_CONTROLLER_AXIS_LEFTX, { RETRO_DEVICE_ID_JOYPAD_LEFT,
                                           RETRO_DEVICE_ID_JOYPAD_RIGHT } },
            { SDL_CONTROLLER_AXIS_LEFTY, { RETRO_DEVICE_ID_JOYPAD_UP,
                                           RETRO_DEVICE_ID_JOYPAD_DOWN } }
        };
        auto axis = static_cast<SDL_GameControllerAxis>(caxis->axis);
        if (mapping.contains(axis)) {
            auto buttons_id = mapping.value(axis);
            setState(buttons_id.first, (caxis->value < -threshold) ? true : false);
            setState(buttons_id.second, (caxis->value > threshold) ? true : false);
        }
    }
    return true;
}

bool Joystick::handleSDLEvent(const SDL_Event *event)
{
    switch (event->type) {
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_JOYDEVICEADDED:
            if (!device_attached)
                return deviceAdded(event);
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
        case SDL_JOYDEVICEREMOVED:
            if (device_attached)
                return deviceRemoved(event);
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            controllerButtonChanged(event);
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
//            joyButtonChanged(event);
            break;

        case SDL_CONTROLLERAXISMOTION:
            controllerAxisChanged(event);
            break;

        case SDL_JOYAXISMOTION:
        case SDL_JOYHATMOTION:
            break;
    }
    return false;
}

QVariant Joystick::Mapping::setMappingOnInput(retro_device_id id, QJSValue cb)
{
    // use a smart pointer so we can capture Connection in the lambda and not
    // have to handle deletion ourselves.
    // Ideally, we would use a unique_ptr but it'll only be possible in C++14
    // which introduces lambda captures expressions.
    auto conn = std::make_shared<QMetaObject::Connection>();

    auto handleInput = [this, id, cb, conn] (InputDeviceEvent *ev, int16_t val) mutable {
        if (val == 0)
            return;

        this->setMapping(ev->clone(), id);
//        if (cb.isCallable())
//            cb.call({ id, ev });
        QObject::disconnect(*conn);
    };

    if (!joystick) {
        // no std::make_unique in c++11
        joystick = std::unique_ptr<Joystick>(new Joystick(this));
    }
    *conn = connect(joystick.get(), &InputDevice::inputEventReceived, handleInput);
    qCDebug(phxInput) << "waiting joystick for input";
    return QVariant::fromValue(*conn);
}

void Joystick::Mapping::cancelMappingOnInput(QVariant cancelInfo)
{
    if (cancelInfo.canConvert<QMetaObject::Connection>())
        QObject::disconnect(cancelInfo.value<QMetaObject::Connection>());
}

bool Joystick::Mapping::populateFromSettings(QSettings &settings)
{
    bool ret = InputDeviceMapping::populateFromSettings(settings);
    if (!ret)
        return ret;

    QVariant guid_ = settings.value("sdl_joystick_guid");
    if (guid_.isValid()) {
        QByteArray guid = guid_.toString().toLatin1();
        joystick_guid = SDL_JoystickGetGUIDFromString(guid.constData());
    }
    return ret;
}

bool Joystick::Mapping::populateFromDict(QVariantMap deviceinfo)
{
    bool ret = InputDeviceMapping::populateFromDict(deviceinfo);
    if (!ret)
        return ret;

    QByteArray guid = deviceinfo["guid"].toString().toLatin1();
    joystick_guid = SDL_JoystickGetGUIDFromString(guid.constData());
    return ret;
}

#define CONVERT_STRING_TO_EVENT(eventType)            \
    do {                                              \
        eventType ev = eventType::fromString(evname); \
        if (ev.isValid())                             \
            return ev.clone();                        \
    } while(false)

InputDeviceEvent *Joystick::Mapping::eventFromString(QString evname)
{
    CONVERT_STRING_TO_EVENT(ControllerButtonEvent);
    CONVERT_STRING_TO_EVENT(ControllerAxisEvent);
    return nullptr;
}

bool Joystick::Mapping::matchJoystick(const SDL_JoystickGUID &guid) const
{
    // XXX: for now, if our guid wasn't defined, just accept every joystick
    char zeroes[sizeof(joystick_guid.data)] = {0};
    if (!memcmp(joystick_guid.data, zeroes, sizeof(joystick_guid.data)))
        return true;

    return !memcmp(joystick_guid.data, guid.data, sizeof(joystick_guid.data));
}
