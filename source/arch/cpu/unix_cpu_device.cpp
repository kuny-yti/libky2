#include "unix_cpu_device.h"

static const uint32 default_max_count = 1024;

inline static const char* parse_number(const char* start, const char* end, uint32 *number_ptr)
{
    uint32 number = 0;
    const char* parsed = start;
    for (; parsed != end; parsed++)
    {
        const uint32 digit = (uint32) (uint8) (*parsed) - (uint32) '0';
        if (digit >= 10)
            break;
        number = number * 10 + digit;
    }
    *number_ptr = number;
    return parsed;
}
inline static bool is_whitespace(char c)
{
    switch (c)
    {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return true;
        default:
            return false;
    }
}
static bool u32_parser(const char* text_start, const char* text_end, void* context)
{
    if (text_start == text_end)
        return false;

    uint32_t kernel_max = 0;
    const char* parsed_end = parse_number(text_start, text_end, &kernel_max);
    if (parsed_end == text_start)
        return false;
    else
    {
        for (const char* char_ptr = parsed_end; char_ptr != text_end; char_ptr++)
        {
            if (!is_whitespace(*char_ptr))
                break;
        }
    }

    uint32* kernel_max_ptr = (uint32*) context;
    *kernel_max_ptr = kernel_max;
    return true;
}
static bool processor_number_parser(uint32 processor_list_start, uint32 processor_list_end, void* context)
{
    uint32* processor_number_ptr = (uint32*) context;
    const uint32 processor_list_last = processor_list_end - 1;
    if (*processor_number_ptr < processor_list_last)
        *processor_number_ptr = processor_list_last;

    return true;
}

typedef bool (*smallfile_callback)(const char*, const char*, void*);
static bool unix_parse_small_file(const char* filename, size_t buffer_size, smallfile_callback callback, void* context)
{
    int file = ::open(filename, O_RDONLY);
    if (file == -1)
        return false;

    size_t buffer_pos = 0;
    ssize_t bytes_read;
    char buffer[buffer_size];
    do
    {
        bytes_read = ::read(file, &buffer[buffer_pos], buffer_size - buffer_pos);
        if (bytes_read < 0)
        {
            ::close(file);
            return false;
        }
        buffer_pos += (size_t) bytes_read;
        if (buffer_pos >= buffer_size)
        {
            ::close(file);
            return false;
        }
    } while (bytes_read != 0);

    ::close(file);
    return callback(buffer, &buffer[buffer_pos], context);
}

typedef bool (*cpulist_callback)(uint32_t, uint32_t, void*);
static bool parse_entry(const char* entry_start, const char* entry_end, cpulist_callback callback, void* context)
{
    /* Skip whitespace at the beginning of an entry */
    for (; entry_start != entry_end; entry_start++)
    {
        if (!is_whitespace(*entry_start))
            break;
    }
    /* Skip whitespace at the end of an entry */
    for (; entry_end != entry_start; entry_end--)
    {
        if (!is_whitespace(entry_end[-1]))
            break;
    }

    const size_t entry_length = (size_t) (entry_end - entry_start);
    if (entry_length == 0)
        return false;

    uint32_t first_cpu, last_cpu;

    const char* number_end = parse_number(entry_start, entry_end, &first_cpu);
    if (number_end == entry_start)
    {
        /* Failed to parse the number; ignore the entry */
        return false;
    }
    else if (number_end == entry_end)
    {
        /* Completely parsed the entry */
        return callback(first_cpu, first_cpu + 1, context);
    }

    /* Parse the second part of the entry */
    if (*number_end != '-')
        return false;

    const char* number_start = number_end + 1;
    number_end = parse_number(number_start, entry_end, &last_cpu);
    if (number_end == number_start)
    {
        /* Failed to parse the second number; ignore the entry */
        return false;
    }

    if (number_end != entry_end)
    {
        /* Partially parsed the entry; ignore unparsed characters and continue with the parsed part */
    }

    if (last_cpu < first_cpu)
        return false;

    /* Parsed both parts of the entry; update CPU set */
    return callback(first_cpu, last_cpu + 1, context);
}
static bool unix_parse_cpulist(const char* filename, cpulist_callback callback, void* context)
{
    bool status = true;
    char buffer[256];

    int file = ::open(filename, O_RDONLY);
    if (file == -1)
        return false;

    size_t position = 0;
    const char* buffer_end = &buffer[256];
    char* data_start = buffer;
    ssize_t bytes_read;
    do
    {
        bytes_read = ::read(file, data_start, (size_t) (buffer_end - data_start));
        if (bytes_read < 0)
        {
            ::close(file);
            return false;
        }

        position += (size_t) bytes_read;
        const char* data_end = data_start + (size_t) bytes_read;
        const char* entry_start = buffer;

        if (bytes_read == 0)
        {
            /* No more data in the file: process the remaining text in the buffer as a single entry */
            const char* entry_end = data_end;
            const bool entry_status = parse_entry(entry_start, entry_end, callback, context);
            status &= entry_status;
        }
        else
        {
            const char* entry_end;
            do
            {
                /* Find the end of the entry, as indicated by a comma (',') */
                for (entry_end = entry_start; entry_end != data_end; entry_end++)
                {
                    if (*entry_end == ',')
                        break;
                }

                /*
                 * If we located separator at the end of the entry, parse it.
                 * Otherwise, there may be more data at the end; read the file once again.
                 */
                if (entry_end != data_end)
                {
                    const bool entry_status = parse_entry(entry_start, entry_end, callback, context);
                    status &= entry_status;
                    entry_start = entry_end + 1;
                }
            } while (entry_end != data_end);

            /* Move remaining partial entry data at the end to the beginning of the buffer */
            const size_t entry_length = (size_t) (entry_end - entry_start);
            ::memmove(buffer, entry_start, entry_length);
            data_start = &buffer[entry_length];
        }
    } while (bytes_read != 0);

    ::close(file);
    return status;
}

uint32 unix_processor_count()
{
    uint32 kernel_max;
    if (unix_parse_small_file(unixCpuKernelMaxFilename, 32, u32_parser, &kernel_max))
        return kernel_max + 1;
    return default_max_count;
}

uint32 unix_processor_possible(uint32 max_count)
{
    uint32 max_possible = 0;
    if (!unix_parse_cpulist(unixCpuPossibleFilename, processor_number_parser, &max_possible))
        return UINT32_MAX;
    if (max_possible >= max_count)
        max_possible = max_count - 1;
    return max_possible;
}

uint32 unix_processor_present(uint32 max_count)
{
    uint32 max_present = 0;
    if (!unix_parse_cpulist(unixCpuPresentFilename, processor_number_parser, &max_present))
        return UINT32_MAX;

    if (max_present >= max_count)
        max_present = max_count - 1;

    return max_present;
}
