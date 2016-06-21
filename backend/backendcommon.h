#pragma once

#ifdef __cplusplus

#include <QtCore>
#include <QtQml>
#include <QtQuick>
#include <QtMultimedia>

#include <atomic>
#include <memory>
#include <string>

#include <math.h>

#include "samplerate.h"
#include "SDL.h"
#include "SDL_gamecontroller.h"

typedef QMap<QString, QString> QStringMap;
Q_DECLARE_METATYPE( QStringMap )

#endif
