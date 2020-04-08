#ifndef KY_ALGORLTHM_INL
#define KY_ALGORLTHM_INL

namespace __sort__ {

template<typename T>
void insert( T *a, int64 length)
{
    for (int i = 1; i < (int)length; i++)
        for (int j = i - 1; j >= 0 && a[j] > a[j + 1]; j--)
            ky_swap(a[j], a[j + 1]);
}

template<typename container_iterator>
void insert (container_iterator begin, container_iterator end)
{
    int length = end - begin;
    for (int i = 1; i < (int)length; i++)
        for (int j = i - 1; j >= 0 && *(begin + j) > *(begin + (j + 1)); j--)
            ky_swap(*(begin + j), *(begin + (j + 1)));
}

template<typename T>
void insert2(T* a, int64 alength)
{
    int first = 0, last = 0;
    T b[alength] = {T()};
    b[0] = a[0];
    for (int i = 1; i < (int)alength; i++)
    {
        if (a[i] < b[first])
        {
            first = (first - 1 + alength) % alength;
            b[first] = a[i];
        }
        else if (a[i]>=b[last])
        {
            last++;
            b[last] = a[i];
        }
        else
        {
            int low, high, mid, d;
            low = first, high = last;
            while (low != high)
            {
                d = (high-low+alength) % alength;
                mid = (low + d / 2) % alength;
                if (a[i] < b[mid])
                    high = mid;
                else
                    low = (mid + 1) % alength;
            }
            for (int k = last + 1; k != low; k = (k - 1 + alength) % alength)
                b[k] = b[(k - 1 + alength) % alength];
            b[low] = a[i];
            last++;
        }
    }
    for (int i = 0; i < (int)alength; i++)
        a[i] = b[(i + first) % alength];
}

template<typename T>
void quick(T *begin, T *end)
{
top:
    int length = end - begin;
    if (length < 2)
        return;

    --end;
    T *low = begin, *high = end-1;
    T *pivot = begin + length / 2 ;

    if (*end < *begin)
        ky_swap(*end, *begin);
    if (length == 2)
        return ;

    if (*pivot < *begin)
        ky_swap(*pivot, *begin);
    if (*end < *pivot)
        ky_swap(*end, *pivot);
    if (length == 3)
        return ;

    ky_swap(*pivot, *end);

    while (low < high)
    {
        while (low < high && *low < *end)
            ++low;
        while (high > low && *end < *high)
            --high;

        if (low < high)
        {
            ky_swap(*low, *high);
            ++low;
            --high;
        } else
            break;
    }

    if (*low < *end)
        ++low;

    ky_swap(*end, *low);
    quick(begin, low);

    begin = low + 1;
    ++end;
    goto top;
}

template<typename container_iterator>
void quick(container_iterator begin, container_iterator end)
{
top:
    int span = int(end - begin);
    if (span < 2)
        return;

    --end;
    container_iterator low = begin, high = end - 1;
    container_iterator pivot = begin + span / 2;

    if (*end < *begin)
        ky_swap(*end, *begin);
    if (span == 2)
        return;

    if (*pivot < *begin)
        ky_swap(*pivot, *begin);
    if (*end < *pivot)
        ky_swap(*end, *pivot);
    if (span == 3)
        return;

    ky_swap(*pivot, *end);

    while (low < high)
    {
        while (low < high && *low < *end)
            ++low;
        while (high > low && *end < *high)
            --high;

        if (low < high)
        {
            ky_swap(*low, *high);
            ++low;
            --high;
        } else
            break;
    }

    if (*low < *end)
        ++low;

    ky_swap(*end, *low);
    quick(begin, low);

    begin = low + 1;
    ++end;
    goto top;
}

template <typename T>
inline void _HeapAdjust_(T *H, T s, int64 length)
{
    T tmp  = H[s];
    int64 child = 2 * s+1;
    while (child < length)
    {
        if (child+1 < length && H[child] < H[child+1])
            ++child ;
        if (H[s] < H[child])
        {
            H[s] = H[child];
            s = child;
            child = 2*s+1;
        }
        else
            break;
        H[s] = tmp;
    }
}

template <typename T>
void heap(T *H, int64 length)
{
    for (int64 i = (length -1) / 2 ; i > 0; --i)
        _HeapAdjust_<T>(H, i, length);
    _HeapAdjust_<T>(H, 0, length);
    for (int64 i = length - 1; i > 0; --i)
    {
        bs_swap(H[i], H[0]);
        _HeapAdjust_<T>(H, 0, i);
    }
}

template <typename T>
void bubble (T *r, int64 length)
{
    int low = 0;
    int high = length -1;
    while (low < high)
    {
        for (int j = low; j < high; ++j)
            if (r[j] > r[j+1])
                ky_swap(r[j], r[j+1]);
        --high;
        for (int j = high; j > low; --j)
            if (r[j] < r[j-1])
                ky_swap(r[j], r[j-1]);
        ++low;
    }
}

template <typename T>
inline void _merge_( T *arr, int low, int mid, int high)
{
    int i = low, j = mid+1, k=0;
    T temp[high-low+1]= {T()};
    while (i <= mid && j <= high)
    {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }
    while (i <= mid)
        temp[k++] = arr[i++];
    while (j <= high)
        temp[k++] = arr[j++];
    for(i = low, k = 0; i <= high; i++, k++)
        arr[i] = temp[k];
}
//!
//! @brief bs_msort 归并排序算法
//!
//! @param arr 是待调整的堆数组
//! @param length是数组的长度
//!
template <typename T>
void merge(T *arr, int64 length)
{
    int size = 1, low, mid, high;
    while(size <= (int)length-1)
    {
        low = 0;
        while(low + size <= (int)length-1)
        {
            mid = low + size-1;
            high = mid + size;
            if(high > (int)length-1)
                high = length-1;
            _merge_<T>(arr, low, mid, high);
            low = high+1;
        }
        size *= 2;
    }
}

}

#endif // KY_ALGORLTHM_INL
