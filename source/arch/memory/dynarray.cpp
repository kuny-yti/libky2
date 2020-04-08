
#include "ky_define.h"
#include "ky_memory.h"

//#define dbg(...) do{fprintf (stderr, __VA_ARGS__);fprintf (stderr, "\n");}while(0)

struct header_t : ky_ref
{
    int64 count;
    int64 begin;
    int64 end;
    int   align;

    static const header_t shared_nul;
};
const header_t header_t::shared_nul = {};

enum
{
    HeaderSize = sizeof(header_t)
};

#define hOffset(h, c) ((uint8 *)(h) + HeaderSize + ((c)* (h)->align))

ky_memory::dynarray::dynarray():
    header(nul())
{
}
ky_memory::dynarray::dynarray(const header_t *h):
    header((header_t *)h)
{
}
ky_memory::dynarray::~dynarray()
{

}

bool ky_memory::dynarray::is_nul ()
{
    return (header == nul());
}
bool ky_memory::dynarray::is_nul ()const
{
    return (header == nul());
}
header_t* ky_memory::dynarray::nul()
{
    return &header_t::shared_nul;
}

ky_ref &ky_memory::dynarray::refer()
{
    return *(ky_ref*)header;
}
void ky_memory::dynarray::addref()
{
    header->addref ();
}
bool ky_memory::dynarray::lessref()
{
    return header->lessref ();
}

// detach realloc
header_t* ky_memory::dynarray::reserve(int64 size, int align)
{
    const int64 mem_byte = ky_memory::block_size(size, align, HeaderSize);

    header_t *old = header;
    header_t *lod = 0;

    if (!is_nul () && (align != old->align))
    {
        kyFree(old);
        header = nul ();
    }
    // detach
    if (is_nul ())
        lod = (header_t *)kyMalloc(mem_byte);
    else
        lod = (header_t *)kyRealloc(old, mem_byte);
    if (!lod)
        return 0;

    // 初始化内存块头
    lod->set(ky_ref::ShareableDetach);
    lod->count = size;
    lod->align = align;
    lod->begin = lod->end = 0;//(size / 3);

    header = lod;
    return old;
}

void ky_memory::dynarray::resize(int64 size, int align)
{
    if (is_nul ())
        reserve(size, align);
    if ((header->begin + size) >= header->count)
        expand((header->begin + size) - header->count);

    header->end += size;
}

// dispose
void ky_memory::dynarray::destroy()
{
    if (!is_nul())
    {
        kyFree(header);
    }
    header = nul();
}

// realloc_grow
bool ky_memory::dynarray::expand(int64 growth)
{
    int64 elem_count = 0;
    const int64 mem_byte =
            ky_memory::block_growing(header->count + growth,
                                    header->align, HeaderSize, &elem_count);

    header_t *t = (header_t *)kyRealloc(header, mem_byte);
    if (!t)
        return false;
    header = t;
    header->count = elem_count;
    return true;
}
// detach_grow
header_t* ky_memory::dynarray::detach(int *idx, int count, int align)
{
    header_t *old = header;
    int64 len = old->end - old->begin;
    int64 new_len = len + count;
    int64 elem_count = 0;
    int64 mem_byte = ky_memory::block_growing (new_len, align,
                                               HeaderSize, &elem_count);

    header_t* t = (header_t *)(kyMalloc(mem_byte));
    if (!t)
        return 0;

    t->count = elem_count;
    t->set(ky_ref::ShareableDetach);
    t->align = align;

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

void *ky_memory::dynarray::append(int n)
{
    int e = header->end;
    if (e + n > header->count)
    {
        int b = header->begin;
        if (b - (int)n >= 2 * (int)header->count / 3)
        {
            e -= b;
            ky_memory::copy (hOffset(header, 0), hOffset(header, b),
                             e * header->align);
            header->begin = 0;
        }
        else
            expand(n);
    }
    header->end = e + n;

    return hOffset(header, e);
}

void *ky_memory::dynarray::prepend(int n)
{
    if (header->begin == 0 || header->begin < n)
    {
        if ((header->end +n) >= (header->count / 3))
            expand(n);

        if ((header->end +n) < (header->count / 3))
            header->begin = header->count - 2 * (header->end -n);
        else
            header->begin = header->count - header->end;

        ky_memory::move (hOffset(header, header->begin), hOffset(header, 0),
                         header->end * header->align);
        header->end += header->begin;
    }

    return hOffset(header, (header->begin -= n));
}

void *ky_memory::dynarray::insert(int i, int n)
{
    if (i <= 0)
        return prepend(n);

    int size = count ();
    if (i >= size)
        return append(n);

    bool leftward = false;

    if (header->begin == 0)
    {
        if (header->end == header->count)
            expand(n);
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
        header->begin -= n;
        ky_memory::move (hOffset(header, header->begin),
                         hOffset(header, header->begin + n), i * header->align);
    }
    else
    {
        ky_memory::move (hOffset(header, header->begin +i +n),
                         hOffset(header, header->begin +i),
                         (size - i) * header->align);
        header->end += n;
    }
    return hOffset(header, header->begin +i);
}

void ky_memory::dynarray::remove(int i, int n)
{
    i += header->begin;
    int middle = i + n/2;
    if (middle - header->begin < header->end - middle)
    {
        ky_memory::move (hOffset(header, header->begin +n),
                         hOffset(header, header->begin),
                         (i - header->begin) * header->align);
        header->begin += n;
    }
    else
    {
        ky_memory::move (hOffset(header, i), hOffset(header,i +n),
                   (header->end -i -n) * header->align);
        header->end -= n;
    }
}

void ky_memory::dynarray::move(int from, int n, int to)
{
    if (from == to)
        return;

    from += header->begin;
    to += header->begin;
    void *t = hOffset(header, from);

    if (from < to)
    {
        if ((header->end == header->count) ||
                3 * (to - from) < 2 * (header->end - header->begin))
        {
            ky_memory::move (hOffset(header, from), hOffset(header, from + 1),
                             (to - from) * header->align);
        }
        else
        {
            if (int offset = from - header->begin)
                ky_memory::move (hOffset(header, header->begin + 1),
                                 hOffset(header, header->begin),
                                 offset * header->align);
            if (int offset = header->end - (to + 1))
                ky_memory::move (hOffset(header, to+2), hOffset(header, to+1),
                                 offset * header->align);
            ++header->begin;
            ++header->end;
            ++to;
        }
    }
    else
    {
        if ((header->begin == 0) ||
                3 * (from - to) < 2 * (header->end - header->begin))
            ky_memory::move (hOffset(header, to+1), hOffset(header, to),
                             (from - to) * header->align);
        else
        {
            if (int offset = to - header->begin)
                ky_memory::move(hOffset(header, header->begin -1),
                                hOffset(header, header->begin),
                                offset * header->align);
            if (int offset = header->end - (from + 1))
                ky_memory::move(hOffset(header, from), hOffset(header, from+1),
                                offset * header->align);
            --header->begin;
            --header->end;
            --to;
        }
    }

    ky_memory::copy (hOffset(header, to), t, header->align);
}


int64 ky_memory::dynarray::count() const
{
    return header->end - header->begin;
}

int64 ky_memory::dynarray::capacity()const
{
    return header->count;
}
bool ky_memory::dynarray::is_empty() const
{
    return header->end == header->begin;
}
void *ky_memory::dynarray::at(int i)
{
    return hOffset(header, header->begin + i);
}
void *ky_memory::dynarray::at(int i) const
{
    return hOffset(header, header->begin + i);
}
void *ky_memory::dynarray::begin()
{
    return hOffset(header, header->begin);
}
void *ky_memory::dynarray::begin()const
{
    return hOffset(header, header->begin);
}
void *ky_memory::dynarray::end()
{
    return hOffset(header, header->end);
}
void *ky_memory::dynarray::end()const
{
    return hOffset(header, header->end);
}
