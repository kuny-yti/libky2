
include (./path.pri)
ky2ArchPath = $${ky2SourcePath}/arch

CPUHeader += \
    $${ky2ArchPath}/cpu/x86_intel_table.h \
    $${ky2ArchPath}/cpu/x86_amd_table.h \
    $${ky2ArchPath}/cpu/x86_other_table.h \
    $${ky2ArchPath}/cpu/x86_cache_table.h \
    $${ky2ArchPath}/cpu/x86_feature_table.h \
    $${ky2ArchPath}/cpu/x86_power_table.h \
    $${ky2ArchPath}/cpu/unix_cpu_device.h \
    $${ky2ArchPath}/cpu/x86_cpu_ability.h
    #$${ky2ArchPath}/cpu/arm_cpu_ability.h \
    #$${ky2ArchPath}/cpu/arm_feature_table.h

CPUSource += \
    $${ky2ArchPath}/cpu/unix_cpu_device.cpp \
    $${ky2ArchPath}/cpu/x86_cpu_ability.cpp \
    #$${ky2ArchPath}/cpu/arm_cpu_ability.cpp \
    $${ky2ArchPath}/cpu/ky_atomic.cpp \
    $${ky2ArchPath}/cpu/ky_atomic_armv6.cpp \
    $${ky2ArchPath}/cpu/ky_atomic_x86.cpp

MemoryHeader += \

MemorySource += \
    $${ky2ArchPath}/memory/dynaddr.cpp \
    $${ky2ArchPath}/memory/dynarray.cpp \
    $${ky2ArchPath}/memory/virmemory.cpp \

HEADERS += \
    $${CPUHeader} \
    $${MemoryHeader}

SOURCES += \
    $${CPUSource} \
    $${MemorySource} \
    $${ky2ArchPath}/ky_cpu.cpp \
    $${ky2ArchPath}/ky_timer.cpp \
    $${ky2ArchPath}/ky_memory.cpp

