#include "scannerutil.h"

void Library::setBackgroundIOPriority() {
#ifdef Q_OS_WIN
    SetThreadPriority( GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN );
#endif

#ifdef Q_OS_MACX
    setiopolicy_np( IOPOL_TYPE_DISK, IOPOL_SCOPE_THREAD, IOPOL_THROTTLE );
#endif

#ifdef Q_OS_LINUX
    //syscall( SYS_ioprio_set, IOPRIO_WHO_PROCESS, 0, IOPRIO_PRIO_VALUE( IOPRIO_CLASS_IDLE, 0) );
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
    //syscall( SYS_ioprio_set, IOPRIO_WHO_PROCESS, 0, IOPRIO_PRIO_VALUE( IOPRIO_CLASS_BE, 0) );
#endif
}
