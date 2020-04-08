
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_alloc.h
 * @brief    关于内存分配器模板实现
 *       1.根据体系架构选择对齐模式
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2020/02/10
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2020/02/10 | 1.0.0.1   | kunyang  | 创建文件
 */

#ifndef ky_ALLOC_H
#define ky_ALLOC_H
#include "ky_define.h"
#include "arch/ky_memory.h"

template <typename T, typename Allocator>
struct ky_allocate_proxy
{
public:
    struct heap
    {
        void* allocate(uint64 size)
        {
            return ::malloc(size);
        }

        void deallocate(void* p, uint64 kyUnused(size))
        {
            return ::free(p);
        }
    };

    template<uint64 align = kyAllocateAlign>
    struct aligned
    {
        void* allocate(size_t size)
        {
            return ky_memory::aligned_alloc(size, align);
        }

        void deallocate(void* p, size_t kyUnused(size))
        {
            return ky_memory::aligned_free(p);
        }
    };

    template<ky_memory::PageType type = ky_memory::Default,
             int prot = ky_memory::Read | ky_memory::Write>
    struct virtual_memory
    {
        void* allocate(uint64 size)
        {
            return ky_memory::virtual_mem::Allocate(size, pageType, prot);
        }

        void deallocate(void* p, uint64 size)
        {
            ky_memory::virtual_mem::Free(p, size);
        }
    };

    template<uint64 commit = kyMmuLargePageSize, ky_memory::PageType type = ky_memory::Default,
             int prot = ky_memory::Read | ky_memory::Write>
    struct address_space
    {
        void* allocate(size_t size)
        {
            return ky_memory::virtual_mem::ReserveAddressSpace(size, commit, type, prot);
        }

        void deallocate(void* p, size_t size)
        {
            ky_memory::virtual_mem::ReleaseAddressSpace(p, size);
        }
    };

public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef uint64 size_type;
    typedef intptr difference_type;

    template<typename U>
    struct rebind
    {
        typedef ky_allocate_proxy<U, Allocator> other;
    };

    // (required to be declared by boost::unordered_map, but should never be called)
    explicit ky_allocate_proxy();

    explicit ky_allocate_proxy(Allocator& allocator)
        : allocator(&allocator)
    {
    }

    template<typename U, class A>
    ky_allocate_proxy(const ky_allocate_proxy<U, A>& rhs)
        : allocator(rhs.allocator)
    {
    }

    bool operator==(const ky_allocate_proxy& rhs) const
    {
        return allocator == rhs.allocator;
    }
    bool operator!=(const ky_allocate_proxy& rhs) const
    {
        return !operator==(rhs);
    }

    pointer address(reference r)
    {
        return &r;
    }

    const_pointer address(const_reference s)
    {
        return &s;
    }

    size_type max_size() const throw ()
    {
        return UINT64_MAX / sizeof(T);
    }

    void construct(const pointer ptr, const value_type& t)
    {
        new(ptr) T(t);
    }

    void destroy(pointer ptr)
    {
        ptr->~T();
        kyUnused2(ptr);
    }

    pointer allocate(size_type n)
    {
        if(n == 0)
            n = 1;
        return (pointer)allocator->allocate(n*sizeof(T));
    }

    pointer allocate(size_type n, const void* const)
    {
        return allocate(n);
    }

    void deallocate(const pointer ptr, const size_type n)
    {
        return allocator->deallocate(ptr, n*sizeof(T));
    }

    Allocator* allocator;
};

struct ky_storage
{
public:
    // 固定存储分配器
    template<typename Allocator = ky_allocate_proxy::aligned<> >
    struct fixed : public ky_noncopy
    {
    public:
        fixed(uint64 size)
            : maxCapacity(size)
            , storage(allocator.allocate(maxCapacity))
        {
        }

        ~fixed()
        {
            allocator.deallocate(storage, maxCapacity);
        }

        uintptr Address() const
        {
            return uintptr(storage);
        }

        uint64 Capacity() const
        {
            return maxCapacity;
        }

        uint64 MaxCapacity() const
        {
            return maxCapacity;
        }

        bool Expand(uint64 kyUnused(reqCapacity))
        {
            return false;
        }

    private:
        Allocator allocator;
        uint64    maxCapacity;
        void*     storage;
    };

    // 通过分配更大的存储空间和复制来进行无限扩展
    template<typename Allocator = ky_allocate_proxy::heap,
             typename GrowthPolicy = ky_memory::growth_exp<> >
    class reallocate : public ky_noncopy
    {
    public:
        reallocate(uint64 initCapacity) :
            capacity(initCapacity) ,
            storage(allocator.allocate(initCapacity))
        {
        }

        ~reallocate()
        {
            allocator.deallocate(storage, capacity);
        }

        uintptr Address() const
        {
            return uintptr(storage);
        }

        uint64 Capacity() const
        {
            return capacity;
        }

        uint64 MaxCapacity() const
        {
            return UINT64_MAX;
        }

        bool Expand(size_t reqCapacity)
        {
            uint64 newCapacity = ky_max(reqCapacity, GrowthPolicy()(capacity));
            void* newStorage = allocator.allocate(newCapacity);
            if(!newStorage)
                return false;
            ky_memory::copy(newStorage, storage, capacity);
            ky_swap(capacity, newCapacity);
            ky_swap(storage, newStorage);
            allocator.deallocate(newStorage, newCapacity);
            return true;
        }

    private:
        Allocator allocator;
        size_t    capacity;
        void*     storage;
    };

    // 通过提交物理内存扩展到分配的地址空间的限制
    // 这样可以避免复制并减少浪费的物理内存
    template<typename Allocator = ky_allocate_proxy::address_space<>,
             typename GrowthPolicy = ky_memory::growth_exp<2,1> >
    class commit : public ky_noncopy
    {
    public:
        commit(size_t maxCapacity_)
            : maxCapacity(kyAlign<kyMmuPageSize>(maxCapacity_))
            , storage(allocator.allocate(maxCapacity))
            , capacity(0)
        {
        }

        ~commit()
        {
            allocator.deallocate(storage, maxCapacity);
        }

        uintptr Address() const
        {
            return uintptr(storage);
        }

        uint64 Capacity() const
        {
            return capacity;
        }

        uint64 MaxCapacity() const
        {
            return maxCapacity;
        }

        bool Expand(uint64 reqCapacity)
        {
            uint64 newCapacity = ky_max(reqCapacity, GrowthPolicy()(capacity));
            // 通过准确反映实际容量来减少昂贵的提交次数
            // 这是安全的，因为我们还对maxCapacity进行了舍入
            newCapacity = kyAlign<kyMmuPageSize>(newCapacity);
            if(newCapacity > maxCapacity)
                return false;
            if(!ky_memory::virtual_mem::Commit(Address()+capacity, newCapacity-capacity))
                return false;
            capacity = newCapacity;
            return true;
        }

    private:
        Allocator allocator;
        uint64 maxCapacity;	// must be initialized before storage
        void* storage;
        uint64 capacity;
    };

    // 当首次访问页面时，通过提交物理内存来隐式扩展到分配的地址空间的限制。
    // 这基本上与带有Growth_Linear的Storage_Commit等效，只是不需要调用Expand
    template<typename Allocator = ky_allocate_proxy::address_space<> >
    class commit_auto : public ky_noncopy
    {
    public:
        commit_auto(uint64 maxCapacity_)
            : maxCapacity(kyAlign<kyMmuPageSize>(maxCapacity_))
            , storage(allocator.allocate(maxCapacity))
        {
            ky_memory::virtual_mem::BeginOnDemandCommits();
        }

        ~commit_auto()
        {
            ky_memory::virtual_mem::EndOnDemandCommits();
            allocator.deallocate(storage, maxCapacity);
        }

        uintptr Address() const
        {
            return uintptr(storage);
        }

        size_t Capacity() const
        {
            return maxCapacity;
        }

        size_t MaxCapacity() const
        {
            return maxCapacity;
        }

        bool Expand(size_t kyUnused(reqCapacity))
        {
            return false;
        }

    private:
        Allocator allocator;
        size_t maxCapacity;	// must be initialized before storage
        void* storage;
    };

public:
    // @return 起始地址（对齐方式取决于分配器）
    uintptr Address() const;

    // @return 当前可访问内存的大小
    uint64 Capacity() const;

    // @return 最大可能的容量
    uint64 MaxCapacity() const;
    // 扩展到至少 reqCapacity,可能更多，具体取决于 GrowthPolicy
    bool Expand(uint64 reqCapacity);

    //!
    //! \brief StorageAppend 保留并在存储末尾返回指向空间的指针，如果需要的话可以扩展它
    //! \param storage
    //! \param end 先前保留的字节总数； 如果分配成功，将增加大小
    //! \param size 预留
    //! \return 分配空间的地址，如果存储空间已满并且无法进一步扩展，则返回0
    //!
    template<class Storage>
    static inline uintptr StorageAppend(Storage& storage, uint64& end, uint64 size)
    {
        uint64 newEnd = end + size;
        if(newEnd > storage.Capacity())
        {
            if(!storage.Expand(newEnd))
                return 0;
        }

        ky_swap(end, newEnd);
        return storage.Address() + newEnd;
    }

    // 在默认构造的Functor实例上调用operator（）以实现Storage及其参数的合理组合
    template<template<class Storage> class Functor>
    static void ForEachStorage()
    {
        Functor<Storage::fixed<ky_allocate_proxy::heap> >()();
        Functor<Storage::fixed<ky_allocate_proxy::aligned<> > >()();

        Functor<Storage::reallocate<ky_allocate_proxy::heap, ky_memory::growth_linear<> > >()();
        Functor<Storage::reallocate<ky_allocate_proxy::heap, ky_memory::growth_exp<> > >()();
        Functor<Storage::reallocate<ky_allocate_proxy::aligned<>, ky_memory::growth_linear<> > >()();
        Functor<Storage::reallocate<ky_allocate_proxy::aligned<>, ky_memory::growth_exp<> > >()();

        Functor<Storage::commit<ky_allocate_proxy::address_space<>, ky_memory::growth_linear<> > >()();
        Functor<Storage::commit<ky_allocate_proxy::address_space<>, ky_memory::growth_exp<> > >()();

        Functor<Storage::commit_auto<> >()();
    }
};

template <typename T>
struct ky_allocate
{
    template<class T>
    class aligned
    {
    public:
        // type definitions
        typedef T        value_type;
        typedef T*       pointer;
        typedef const T* const_pointer;
        typedef T&       reference;
        typedef const T& const_reference;
        typedef uint64   size_type;
        typedef intptr   difference_type;

        // rebind allocator to type U
        template <typename U>
        struct rebind
        {
            typedef aligned<U> other;
        };

        pointer address(reference value) const
        {
            return &value;
        }

        const_pointer address(const_reference value) const
        {
            return &value;
        }

        aligned()
        {
        }

        aligned(const aligned&)
        {
        }

        template <class U>
        aligned (const aligned<U>&)
        {
        }

        ~aligned()
        {
        }

        size_type max_size() const
        {
            // maximum number of *elements* that can be allocated
            return INT64_MAX / sizeof(T);
        }

        // allocate uninitialized storage
        pointer allocate(size_type numElements)
        {
            const size_type alignment = x86_x64::Caches(x86_x64::L1D)->entrySize;
            const size_type elementSize = ky_round_up(sizeof(T), alignment);
            const size_type size = numElements * elementSize;
            pointer p = (pointer)ky_memory::aligned_alloc(size, alignment);
            return p;
        }

        // deallocate storage of elements that have been destroyed
        void deallocate(pointer p, size_type kyUnused(num))
        {
            ky_memory::aligned_free((void*)p);
        }

        void construct(pointer p, const T& value)
        {
            new((void*)p) T(value);
        }

        void destroy(pointer p)
        {
            p->~T();
            kyUnused2(p);
        }
    };

    // indicate that all specializations of this allocator are interchangeable
    template <class T1, class T2>
    bool operator==(const aligned<T1>&, const aligned<T2>&)
    {
        return true;
    }

    template <class T1, class T2>
    bool operator!=(const aligned<T1>&, const aligned<T2>&)
    {
        return false;
    }


    /**
     * 分配器设计的参数
     * - O(1) 分配;
     * - 可变大小的块;
     * - 支持取消分配所有对象;
     * - 连续分配是连续的;
     * - 没有额外的对齐也没有填充.
     **/
    template<class Storage = ky_storage::fixed<> >
    class arena : public ky_noncopy
    {
    public:
        arena(uint64 maxSize):
            storage(maxSize)
        {
            DeallocateAll();
        }

        uint64 remaining() const
        {
            return storage.MaxCapacity() - end;
        }

        void* allocate(uint64 size)
        {
            return (void*)StorageAppend(storage, end, size);
        }

        void deallocate(void* kyUnused(p), uint64 kyUnused(size))
        {
            // ignored
        }

        void DeallocateAll()
        {
            end = 0;
        }

        // 返回地址是否在先前分配的范围内
        bool Contains(uintptr address) const
        {
            return (address - storage.Address()) < end;
        }

    private:
        Storage storage;
        uint64  end;
    };

    /**
     * 分配器设计的参数
     * - 以固定的chunkSize动态增长;
     * - 用于频繁分配大小 << chunkSize;
     * - 没有重新分配，指针仍然有效
     **/
    class arena_dynamic : public ky_noncopy
    {
        struct ArenaChunk
        {
            bool Available(uint64 size) const
            {
                return size <= (capacity - end);
            }

            uintptr Allocate(uint64 size)
            {
                uintptr ptr = storage + end;
                end += size;
                return ptr;
            }

            uintptr storage;
            uint64 end;
            uint64 capacity;
            ArenaChunk* next;
        };

    public:
        arena_dynamic(uint64 chunkSize) :
            chunkSize(chunkSize),
            head(0)
        {
            AllocateNewChunk();
        }

        ~arena_dynamic()
        {
            ArenaChunk* chunk = head;
            while (chunk != 0)
            {
                ArenaChunk* next = chunk->next;
                ::free(chunk);
                chunk = next;
            }
        }

        void AllocateNewChunk()
        {
            // For efficiency, do a single allocation with the ArenaChunk and its storage
            ArenaChunk* next = head;
            head = (ArenaChunk*)::malloc(sizeof(ArenaChunk) + chunkSize);
            //ENSURE(head);
            head->storage = sizeof(ArenaChunk) + uintptr(head);
            head->end = 0;
            head->capacity = chunkSize;
            head->next = next;
        }

        void* allocate(uint64 size)
        {
            if (size > chunkSize)
            {
                //debug_warn(L"DynamicArena cannot allocate more than chunk size");
                //throw std::bad_alloc();
            }
            else if (!head->Available(size))
                AllocateNewChunk();

            return (void*)head->Allocate(size);
        }

        void deallocate(void* kyUnused(p), uint64 kyUnused(size))
        {
            // ignored
        }

    private:

        const uint64 chunkSize;
        ArenaChunk  *head;
    };

};

#endif
