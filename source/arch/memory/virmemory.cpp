#include "arch/ky_memory.h"

#if !kyOSIsWin32

# include <sys/mman.h>
#ifndef MAP_ANONYMOUS
# define MAP_ANONYMOUS MAP_ANON
#endif

static const int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS;


void* ky_memory::virtual_mem::ReserveAddressSpace(uint64 size, uint64 kyUnused(commit),
                                                  PageType kyUnused(type), int kyUnused(prot))
{
    errno = 0;
    void* p = ::mmap(0, size, PROT_NONE, mmap_flags | MAP_NORESERVE, -1, 0);
    if(p == MAP_FAILED)
        return 0;
    return p;
}

void ky_memory::virtual_mem::ReleaseAddressSpace(void* p, uint64 size)
{
    //ENSURE(size != 0);

    errno = 0;
    if(::munmap(p, size) != 0)
    {
        //DEBUG_WARN_ERR(StatusFromErrno());
    }
}


bool ky_memory::virtual_mem::Commit(uintptr address, uint64 size, PageType kyUnused(type), int prot)
{
    if(prot == PROT_NONE)	// would be understood as a request to decommit
    {
        //DEBUG_WARN_ERR(ERR::INVALID_PARAM);
        return false;
    }

    errno = 0;
    if(::mmap((void*)address, size, prot, mmap_flags|MAP_FIXED, -1, 0) == MAP_FAILED)
        return false;

    if(prot != (PROT_READ | PROT_WRITE))
        (void)Protect(address, size, prot);

    return true;
}

bool ky_memory::virtual_mem::Decommit(uintptr address, uint64 size)
{
    errno = 0;
    if(::mmap((void*)address, size, PROT_NONE,
              mmap_flags | MAP_NORESERVE | MAP_FIXED, -1, 0) == MAP_FAILED)
        return false;
    return true;
}


bool ky_memory::virtual_mem::Protect(uintptr address, uint64 size, int prot)
{
    errno = 0;
    if(::mprotect((void*)address, size, prot) != 0)
    {
        //DEBUG_WARN_ERR(ERR::FAIL);
        return false;
    }
    return true;
}


void* ky_memory::virtual_mem::Allocate(uint64 size, PageType type, int prot)
{
    void* p = ReserveAddressSpace(size);
    if(!p)
        return 0;

    if(!Commit(uintptr(p), size, type, prot))
    {
        ReleaseAddressSpace(p, size);
        return 0;
    }

    return p;
}

void ky_memory::virtual_mem::Free(void* p, uint64 size)
{
    // (only the Windows implementation distinguishes between Free and ReleaseAddressSpace)
    ReleaseAddressSpace(p, size);
}


void ky_memory::virtual_mem::BeginOnDemandCommits()
{
    // not yet implemented, but possible with a signal handler
}

void ky_memory::virtual_mem::EndOnDemandCommits()
{
    // not yet implemented, but possible with a signal handler
}


void ky_memory::virtual_mem::DumpStatistics()
{
    // we haven't collected any statistics
}

#else

#endif
