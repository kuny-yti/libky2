

#include "ky_define.h"
#include "ky_memory.h"

struct header_t : ky_ref
{
    int32 count;
    int32 begin;
    int32 end;
    void *addr[1];

    static const header_t shared_nul;
};
const header_t header_t::shared_nul = {};

enum
{
    HeaderSize = sizeof(header_t) - ky_memory::dynaddr::NodeSize
};


ky_memory::dynaddr::dynaddr():
    header(nul())
{
}
ky_memory::dynaddr::dynaddr(const header_t *h):
    header((header_t *)h)
{
}
ky_memory::dynaddr::~dynaddr()
{
    destroy();
}

bool ky_memory::dynaddr::is_nul ()
{
    return header == nul();
}
bool ky_memory::dynaddr::is_nul ()const
{
    return header == nul();
}
header_t* ky_memory::dynaddr::nul()
{
    return &header_t::shared_nul;
}

// detach realloc
header_t* ky_memory::dynaddr::reserve(int64 size)
{
    const int64 mem_byte = ky_memory::block_size(size, NodeSize, HeaderSize);

    header_t *old = header;
    // detach
    if (is_nul ())
    {
        header_t* t = (header_t *)kyMalloc(mem_byte);
        if (!t)
            return 0;

        t->set(ky_ref::ShareableDetach);
        t->count = size;
        t->begin = 0;
        t->end = 0;
        header = t;
    }
    // realloc
    else
    {
        header_t *t = (header_t *)kyRealloc(old, mem_byte);
        if(t)
            return 0;

        header = t;
        header->count = size;
        if (!size)
            header->begin = header->end = 0;
    }
    return old;
}

// dispose
void ky_memory::dynaddr::destroy()
{
    if (!is_nul())
        kyFree(header);
    header = nul();
}

// realloc_grow
bool ky_memory::dynaddr::expand(int64 growth)
{
    int64 elem_count = 0;
    const int64 mem_byte =
            ky_memory::block_growing(count() + growth,
                                    NodeSize, HeaderSize, &elem_count);

    header_t *t = (header_t *)kyRealloc(header, mem_byte);
    if (!t)
        return false;
    header = t;
    header->count = elem_count;
    return true;
}
// detach_grow
header_t* ky_memory::dynaddr::detach(int *idx, int count)
{
    header_t *old = header;
    int64 len = old->end - old->begin;
    int64 new_len = len + count;
    int64 elem_count = 0;
    int64 mem_byte = ky_memory::block_growing (new_len, NodeSize, HeaderSize, &elem_count);

    header_t* t = (header_t *)(kyMalloc(mem_byte));
    if (!t)
        return 0;

    t->count = elem_count;
    t->set(ky_ref::ShareableDetach);

    int begin = 0;
    if (*idx < 0)
    {
        *idx = 0;
        begin = (t->count - new_len) >> 1;
    }
    else if (*idx > len)
    {
        *idx = len;
        begin = 0;
    }
    else if (*idx < (len >> 1))
        begin = (t->count - new_len) >> 1;

    t->begin = begin;
    t->end = begin + new_len;
    header = t;
    return old;
}

ky_memory::dynaddr::node_t *ky_memory::dynaddr::append(int n)
{
    int e = header->end;
    if (e + n > header->count)
    {
        int b = header->begin;
        if (b - (int)n >= 2 * (int)header->count / 3)
        {
            e -= b;
            ky_memory::copy (header->addr, header->addr + b, e * NodeSize);
            header->begin = 0;
        }
        else
            expand(n);
    }
    header->end = e + n;
    return (node_t*)header->addr + e;
}

ky_memory::dynaddr::node_t *ky_memory::dynaddr::prepend(int n)
{
    if (header->begin == 0)
    {
        if (header->end >= header->count / 3)
            expand(1);

        if (header->end < header->count / 3)
            header->begin = header->count - 2 * header->end;
        else
            header->begin = header->count - header->end;

        ky_memory::move (header->addr + header->begin, header->addr, header->end * NodeSize);
        header->end += header->begin;
    }
    return (node_t*)header->addr + (--header->begin);
}

ky_memory::dynaddr::node_t *ky_memory::dynaddr::insert(int i, int n)
{
    if (i <= 0)
        return prepend(n);

    int size = header->end - header->begin;
    if (i >= size)
        return append(n);

    bool leftward = false;

    if (header->begin == 0)
    {
        if (header->end == header->count)
            expand(1);
    }
    else
    {
        if (header->end == header->count)
            leftward = true;
        else
            leftward = (i < size - i);
    }

    if (leftward)
    {
        --header->begin;
        ky_memory::move (header->addr + header->begin, header->addr + header->begin + 1, i * NodeSize);
    }
    else
    {
        ky_memory::move (header->addr + header->begin +i+1, header->addr + header->begin +i,
                        (size - i) * NodeSize);
        ++header->end;
    }
    return (node_t*)header->addr + header->begin +i;
}

void ky_memory::dynaddr::remove(int i)
{
    i += header->begin;
    if (i - header->begin < header->end - i)
    {
        if (int offset = i - header->begin)
            ky_memory::move (header->addr + header->begin +1, header->addr + header->begin,
                            offset * NodeSize);
        header->begin++;
    }
    else
    {
        if (int offset = header->end -i-1)
            ky_memory::move (header->addr + i, header->addr +i+1, offset * NodeSize);
        header->end--;
    }
}
void ky_memory::dynaddr::remove(int i, int n)
{
    i += header->begin;
    int middle = i + n/2;
    if (middle - header->begin < header->end - middle)
    {
        ky_memory::move (header->addr + header->begin +n, header->addr + header->begin,
                        (i - header->begin) * NodeSize);
        header->begin += n;
    }
    else
    {
        ky_memory::move (header->addr +i, header->addr +i +n,
                   (header->end -i -n) * NodeSize);
        header->end -= n;
    }
}

void ky_memory::dynaddr::move(int from, int64 n, int to)
{
    if (from == to)
        return;

    from += header->begin;
    to += header->begin;
    void *t = header->addr[from];

    if (from < to)
    {
        if (header->end == header->count || 3 * (to - from) < 2 * (header->end - header->begin))
        {
            ky_memory::move (header->addr + from, header->addr + from + 1, (to - from) * NodeSize);
        }
        else
        {
            if (int offset = from - header->begin)
                ky_memory::move (header->addr + header->begin + 1, header->addr + header->begin,
                                offset * NodeSize);
            if (int offset = header->end - (to + 1))
                ky_memory::move (header->addr +to+2, header->addr +to+1, offset * NodeSize);
            ++header->begin;
            ++header->end;
            ++to;
        }
    }
    else
    {
        if (header->begin == 0 || 3 * (from - to) < 2 * (header->end - header->begin))
            ky_memory::move (header->addr +to+1, header->addr +to, (from - to) * NodeSize);
        else
        {
            if (int offset = to - header->begin)
                ky_memory::move(header->addr + header->begin -1, header->addr + header->begin,
                          offset * NodeSize);
            if (int offset = header->end - (from + 1))
                ky_memory::move(header->addr + from, header->addr +from+1, offset * NodeSize);
            --header->begin;
            --header->end;
            --to;
        }
    }
    header->addr[to] = t;
}

ky_memory::dynaddr::node_t *ky_memory::dynaddr::erase(node_t *xi)
{
    int i = xi - ((node_t*)header->addr + header->begin);
    remove(i);
    return (node_t*)header->addr + header->begin + i;
}

int64 ky_memory::dynaddr::count() const
{
    return header->end - header->begin;
}

int64 ky_memory::dynaddr::capacity()const
{
    return header->count;
}
bool ky_memory::dynaddr::is_empty() const
{
    return header->end == header->begin;
}
ky_memory::dynaddr::node_t *ky_memory::dynaddr::at(int i)
{
    return (node_t*)header->addr + header->begin + i;
}
ky_memory::dynaddr::node_t *ky_memory::dynaddr::at(int i)const
{
    return (node_t*)header->addr + header->begin + i;
}
ky_memory::dynaddr::node_t *ky_memory::dynaddr::begin()
{
    return (node_t*)header->addr + header->begin;
}
ky_memory::dynaddr::node_t *ky_memory::dynaddr::end()
{
    return(node_t*) header->addr + header->end;
}
ky_memory::dynaddr::node_t *ky_memory::dynaddr::begin()const
{
    return (node_t*)header->addr + header->begin;
}
ky_memory::dynaddr::node_t *ky_memory::dynaddr::end()const
{
    return (node_t*)header->addr + header->end;
}
