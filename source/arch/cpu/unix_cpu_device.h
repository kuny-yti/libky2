#ifndef UNIX_CPU_DEVICE_H
#define UNIX_CPU_DEVICE_H

#include "ky_define.h"

static const char *const unixCpuKernelMaxFilename = "/sys/devices/system/cpu/kernel_max";
static const char *const unixCpuPossibleFilename  = "/sys/devices/system/cpu/possible";
static const char *const unixCpuPresentFilename   = "/sys/devices/system/cpu/possible";
static const char *const unixCpuInfoFilename      = "/proc/cpuinfo";

//!
//! \brief unix_processors_count 类unix OS最大处理器数
//! \return
//!
uint32 unix_processor_count();
//!
//! \brief unix_processor_possible 类unix OS处理器可能最大数
//! \param max_count
//! \return
//!
uint32 unix_processor_possible(uint32 max_count);
//!
//! \brief unix_processor_present 类unix OS处理器当前最大数
//! \param max_processor_count
//! \return
//!
uint32 unix_processor_present(uint32 max_count);

#endif
