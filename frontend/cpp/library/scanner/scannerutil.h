#pragma once

#include "frontendcommon.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <tchar.h>
#endif

#ifdef Q_OS_MACX
#include <sys/resource.h>
#endif

#ifdef Q_OS_LINUX

#include <syscall.h>

/*
 * Gives us 8 prio classes with 13-bits of data for each class
 */
#define IOPRIO_BITS     (16)
#define IOPRIO_CLASS_SHIFT  (13)
#define IOPRIO_PRIO_MASK    ((1UL << IOPRIO_CLASS_SHIFT) - 1)

#define IOPRIO_PRIO_CLASS(mask) ((mask) >> IOPRIO_CLASS_SHIFT)
#define IOPRIO_PRIO_DATA(mask)  ((mask) & IOPRIO_PRIO_MASK)
#define IOPRIO_PRIO_VALUE(class, data)  (((class) << IOPRIO_CLASS_SHIFT) | data)

#define ioprio_valid(mask)  (IOPRIO_PRIO_CLASS((mask)) != IOPRIO_CLASS_NONE)

/*
 * These are the io priority groups as implemented by CFQ. RT is the realtime
 * class, it always gets premium service. BE is the best-effort scheduling
 * class, the default for any process. IDLE is the idle scheduling class, it
 * is only served when no one else is using the disk.
 */

enum {
    IOPRIO_CLASS_NONE,
    IOPRIO_CLASS_RT,
    IOPRIO_CLASS_BE,
    IOPRIO_CLASS_IDLE,
};

enum {
    IOPRIO_WHO_PROCESS = 1,
    IOPRIO_WHO_PGRP,
    IOPRIO_WHO_USER,
};

#endif

namespace Library {
    // These helper functions call OS-specific APIs to lower (and restore) the IO priority of the thread that calls them
    void setBackgroundIOPriority();
    void setNormalIOPriority();
}
