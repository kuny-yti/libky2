#include "arch/ky_cpu.h"

#if kyArchIsX86
#include "arch/x86_cpu_ability.h"
#elif kyArchIsARM
#include "arch/arm_cpu_ability.h"
#endif

static cpu_ability *__ability = cpu_ability::instance();

bool ky_cpu::has(eCPUFeatures cap)
{
    return __ability->feature & cap;
}

ky_cpu::cache_t *ky_cpu::cache(const uint &level)
{
    return cache_t::query(level);
}
eCPUVendors ky_cpu::vendor()
{
    return __ability->vendor;
}

int ky_cpu::page_size()
{
    static uint pageSize;
    if (pageSize != 0)
        return pageSize;

#if kyOSIsLinux
    pageSize = (uint)::sysconf(_SC_PAGESIZE);
#elif kyOSIsWin32

#else

#endif
    return pageSize;
}
int ky_cpu::large_page_size()
{
    return 0; // 不支持
}

int ky_cpu::count()
{
    static int nb_cpus = 0;
    if (nb_cpus != 0)
        return nb_cpus;
#if defined(_SC_NPROC_ONLN)
    nb_cpus = ::sysconf(_SC_NPROC_ONLN);
#elif defined(_SC_NPROCESSORS_ONLN)
    nb_cpus = ::sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROCESSORS_CONF)
    nb_cpus = ::sysconf(_SC_NPROCESSORS_CONF);
#elif defined(_SCHED_H) && defined(CPU_COUNT)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    if (!::sched_getaffinity(0, sizeof(cpuset), &cpuset))
        nb_cpus = CPU_COUNT(&cpuset);
#elif kyOSIsWin32
    DWORD_PTR proc_aff, sys_aff;
    if (GetProcessAffinityMask(GetCurrentProcess(), &proc_aff, &sys_aff))
        nb_cpus = (int)proc_aff;//av_popcount64(proc_aff);
#elif defined(HW_NCPU)
    int mib[2] = { CTL_HW, HW_NCPU };
    int64 len = sizeof(nb_cpus);
    if (::sysctl(mib, 2, &nb_cpus, &len, NULL, 0) == -1)
        nb_cpus = 0;
#elif kyOS == kyOS_Phone
    SYSTEM_INFO sysinfo;
    GetNativeSystemInfo(&sysinfo);
    nb_cpus = sysinfo.dwNumberOfProcessors;
#endif

    return nb_cpus;
}
