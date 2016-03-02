#include "scannerutil.h"

void Library::setBackgroundIOPriority() {
#ifdef Q_OS_WIN
    SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN );
#endif

#ifdef Q_OS_MACX
#endif

#ifdef Q_OS_LINUX
#endif
}

void Library::setNormalIOPriority() {
#ifdef Q_OS_WIN
    SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_END );
#endif

#ifdef Q_OS_MACX
#endif

#ifdef Q_OS_LINUX
#endif
}
