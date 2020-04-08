
include (./path.pri)
ky2IoPath = $${ky2SourcePath}/io

ZipHeader = \
    $${ky2IoPath}/zip/unzip.h \
    $${ky2IoPath}/zip/zip.h \
    $${ky2IoPath}/zip/ioapi.h \
    $${ky2IoPath}/zip/crypt.h
ZipSource = \
    $${ky2IoPath}/zip/unzip.c \
    $${ky2IoPath}/zip/zip.c \
    $${ky2IoPath}/zip/ioapi.c

HEADERS += \
    $${ZipHeader} \
    $${ky2IoPath}/generic_io.h \
    $${ky2IoPath}/sync_io.h \
    $${ky2IoPath}/dynamic_io.h \
    $${ky2IoPath}/directory_io.h \
    $${ky2IoPath}/socket_io.h \
    $${ky2IoPath}/zip_io.h

SOURCES += \
    $${ZipSource} \
    $${ky2IoPath}/generic_io.cpp \
    $${ky2IoPath}/sync_io.cpp \
    $${ky2IoPath}/dynamic_io.cpp \
    $${ky2IoPath}/directory_io.cpp \
    $${ky2IoPath}/socket_io.cpp \
    $${ky2IoPath}/zip_io.cpp

