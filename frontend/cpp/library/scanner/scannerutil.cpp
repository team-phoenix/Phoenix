#include "scannerutil.h"

void Library::setBackgroundIOPriority() {
#ifdef Q_OS_WIN
    SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN );
#endif

#ifdef Q_OS_MACX
    setiopolicy_np( IOPOL_TYPE_DISK, IOPOL_SCOPE_THREAD, IOPOL_THROTTLE );
#endif

#ifdef Q_OS_LINUX
#endif
}

void Library::setNormalIOPriority() {
#ifdef Q_OS_WIN
    SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_END );
#endif

#ifdef Q_OS_MACX
    setiopolicy_np( IOPOL_TYPE_DISK, IOPOL_SCOPE_THREAD, IOPOL_IMPORTANT );
#endif

#ifdef Q_OS_LINUX
#endif
}
