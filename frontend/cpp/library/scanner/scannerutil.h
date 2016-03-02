#pragma once

#include "frontendcommon.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <tchar.h>
#endif

#ifdef Q_OS_MACX
#endif

#ifdef Q_OS_LINUX
#endif

namespace Library {
    // These helper functions call OS-specific APIs to lower (and restore) the IO priority of the thread that calls them
    void setBackgroundIOPriority();
    void setNormalIOPriority();
}
