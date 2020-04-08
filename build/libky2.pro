#-------------------------------------------------
#
# Project created by QtCreator 2015-06-25T00:38:08
#
#-------------------------------------------------

include (../bulid_library.pri)
include (./thread.pri)
include (./maths.pri)
include (./tools.pri)
include (./arch.pri)
include (./io.pri)
include (./network.pri)

CONFIG -= qt
TARGET = ky2$${BuildSuffix}
TEMPLATE = lib

contains(Platform, Win32){
    LIBS+=  -lws2_32 -lwsock32 -lz
}
contains(Platform, Linux){
    LIBS += -lz -ldl
}


#CONFIG += -msse4a -mavx2 -mavx -mbmi2 -mbmi -msse2 -mf16c -mfma4 -mfma -mrdrnd -mfsgsbase -mlwp -mlzcnt -m3dnow -mmmx -msse4.2 -msse3 -mpopcnt -msse4 -mtbm -mssse3 -maes -mpclmul -msse -mxop
CONFIG += c++17 c11
QMAKE_CXXFLAGS += -Wno-ambiguous-ellipsis -Wno-unknown-pragmas -Wno-missing-braces

DEFINES += YAML_DECLARE_STATIC

SOURCES += \
    $${ky2SourcePath}/ky_hooks.cpp \
    $${ky2SourcePath}/ky_debug.cpp \
    $${ky2SourcePath}/ky_allocate.cpp \
    $${ky2SourcePath}/ky_object.cpp \
    $${ky2SourcePath}/ky_app.cpp

HEADERS += \




