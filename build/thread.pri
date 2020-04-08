
include (./path.pri)
ky2ThreadPath = $${ky2SourcePath}/thread

HEADERS += \
    $${ky2ThreadPath}/timer_posix.h \
    $${ky2ThreadPath}/signal_posix.h \
    $${ky2ThreadPath}/posix_fd.h \
    $${ky2ThreadPath}/pipe_posix.h  \
    $${ky2ThreadPath}/event_poll.h \
    $${ky2ThreadPath}/thread_dispatch.h

SOURCES += \
    $${ky2ThreadPath}/timer_posix.cpp \
    $${ky2ThreadPath}/signal_posix.cpp \
    $${ky2ThreadPath}/pipe_posix.cpp \
    $${ky2ThreadPath}/event_windows.cpp \
    $${ky2ThreadPath}/event_win32.cpp \
    $${ky2ThreadPath}/event_posix.cpp \
    $${ky2ThreadPath}/thread_dispatch.cpp \
    $${ky2ThreadPath}/ky_lock.cpp \
    $${ky2ThreadPath}/ky_thread.cpp
