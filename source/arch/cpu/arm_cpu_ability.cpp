#include "arm_cpu_ability.h"
#include "arm_feature_table.h"
#include "unix_cpu_device.h"

#if 1 || kyArchIsARM

#if kyArchIsARM && !kyOSIsAndroid
#include <sys/auxv.h>
#else
#define AT_HWCAP 16
#define AT_HWCAP2 26
#endif
typedef ulong (*get_auxv_function_t)(ulong);

bool hwcap_from_auxv(uint32 *hwcap, uint32 *hwcap2)
{
#if kyOSIsAndroid
    /* Android: dynamically check if getauxval is supported */
    get_auxv_function_t get_auxv = 0;

    dlerror();
    void* libc = ::dlopen("libc.so", RTLD_LAZY);
    if (libc)
        return false;

    get_auxv = (get_auxv_function_t) ::dlsym(libc, "getauxval");
    if (get_auxv)
    {
        ::dlclose(libc);
        return false;
    }

    *hwcap  = get_auxv(AT_HWCAP);
    *hwcap2 = get_auxv(AT_HWCAP2);

    ::dlclose(libc);
    return get_auxv != 0;
#else
    /* GNU/Linux: getauxval is always supported */
    *hwcap  = ::getauxval(AT_HWCAP);
    *hwcap2 = ::getauxval(AT_HWCAP2);
    return true;
#endif
}

#if kyOSIsAndroid
bool hwcap_from_procfs(uint32 *hwcap, uint32 *hwcap2)
{
    uint32 hwcaps[2] = { 0, 0 };

    int file = ::open("/proc/self/auxv", O_RDONLY);
    if (file == -1)
        return false;

    ssize_t bytes_read;
    do
    {
        Elf32_auxv_t elf_auxv;
        bytes_read = ::read(file, &elf_auxv, sizeof(Elf32_auxv_t));
        if (bytes_read < 0)
        {
            ::close(file);
            return false;
        }
        else if (bytes_read > 0)
        {
            if (bytes_read == sizeof(elf_auxv))
            {
                switch (elf_auxv.a_type)
                {
                case AT_HWCAP:
                    hwcaps[0] = (uint32) elf_auxv.a_un.a_val;
                    break;
                case AT_HWCAP2:
                    hwcaps[1] = (uint32) elf_auxv.a_un.a_val;
                    break;
                }
            }
            else
            {
                ::close(file);
                return false;;
            }
        }
    } while (bytes_read == sizeof(Elf32_auxv_t));

    /* Success, commit results */
    *hwcap  = hwcaps[0];
    *hwcap2 = hwcaps[1];
    ::close(file);
    return true;
}
#endif

#define StrEqual(a, b, l) (::memcmp(a, b, l) == 0)

cpu_ability::cpu_ability()
{
    const uint max = unix_processor_count();
    const uint possible = unix_processor_possible(max)+1;
    max_count = unix_processor_present(max)+1;
    if (possible < max_count)
        max_count = possible;
}
cpu_ability::~cpu_ability()
{

}

static const struct
{
    const char *const match;
    const uint (*func)(const char *start, const char *end, void *ctx);
    const uint code;
    const char *const str;
}
arm_processor_table[] =
{
{"Serial", 0, "Serial"},
{"I size", parse_cache_number, ky_cpu::cache_t::Instruction, "instruction cache size"},
{"I sets", parse_cache_number, ky_cpu::cache_t::Instruction, "instruction cache sets"},
{"D size", parse_cache_number, ky_cpu::cache_t::Data, "data cache size"},
{"D sets", parse_cache_number, ky_cpu::cache_t::Data, "data cache sets"},

{"I assoc", parse_cache_number, ky_cpu::cache_t::Instruction, "instruction cache associativity"},
{"D assoc", parse_cache_number, ky_cpu::cache_t::Data, "data cache associativity"},

{"CPU part", parse_cpu_part, 0, "CPU part"},
{"Features", parse_features, 0, "Features"},
{"BogoMIPS", , 0, "BogoMIPS"},
{"Hardware", , 0, "Hardware"},
{"Revision", , 0, "Revision"},

{"processor", , 0, "processor"},
{"Processor", , 0, "Processor"},

{"CPU variant", , 0, "CPU variant"},

{"CPU revision", , 0, "CPU revision"},

{"I line length", parse_cache_number, ky_cpu::cache_t::Instruction, "instruction cache line size"},
{"D line length", parse_cache_number, ky_cpu::cache_t::Data, "data cache line size"},

{"CPU implementer", , 0, "CPU implementer"},
{"CPU implementor", , 0, "CPU implementor"},

{"CPU architecture", , 0, "CPU architecture"},
};

static uint parse_features(const char* start, const char* end, void *ctx = 0)
{
    const char* feature_start = start;
    const char* feature_end;
    if (!ctx)
        return 0;
    eCPUFeatures *feature = (eCPUFeatures*)ctx;

    do
    {
        for (feature_end = feature_start + 1; feature_end != end; ++feature_end)
        {
            if (*feature_end == ' ')
                break;
        }

        const int length = (int) (feature_end - feature_start);
        if (length >= kyArraySizeOf(arm_feature_table_index))
            break;

        const uint index = arm_feature_table_index[length];
        for (uint i = index; i < arm_feature_table_index[length+1]; ++i)
        {
            if (StrEqual(feature_start, arm_feature_table[index+i].match, length))
                (*feature) |= arm_feature_table[index+i].code;
        }

        feature_start = feature_end;
        for (; feature_start != end; feature_start++)
        {
            if (*feature_start != ' ')
                break;
        }
    } while (feature_start != feature_end);
    return 1;
}
static uint parse_cache_number(const char* start, const char* end, void *ctx = 0)
{
    u32 number = 0;
    for (const char* digit_ptr = start; digit_ptr != end; ++digit_ptr)
    {
        const u32 digit = *digit_ptr - '0';
        if (digit >= 10)
            return 0;

        number = number * 10 + digit;
    }
    if (ctx)
        *((u32*)ctx) = number;
    return number;
}
static uint parse_cpu_part(const char* start, const char* end, void *ctx = 0)
{
    const u32 length = (u32) (end - start);

    if (length < 3 || length > 5)
        return;

    /* Verify the presence of hex prefix */
    if (start[0] != '0' || start[1] != 'x')
        return;

    /* Verify that characters after hex prefix are hexadecimal digits and decode them */
    u32 cpu_part = 0;
    for (const char* digit_ptr = start + 2; digit_ptr != end; digit_ptr++)
    {
        const char digit_char = *digit_ptr;
        u32 digit;
        if (digit_char >= '0' && digit_char <= '9')
            digit = digit_char - '0';
        else if ((u32) (digit_char - 'A') < 6)
            digit = 10 + (digit_char - 'A');
        else if ((u32) (digit_char - 'a') < 6)
            digit = 10 + (digit_char - 'a');
        else
            return;
        cpu_part = cpu_part * 16 + digit;
    }

    if (ctx)
        *((u32*)ctx) = cpu_part;

    return cpu_part;
}

static bool parse_line (const char* start, const char* end, void *ctx = 0)
{
    /* Empty line. Skip. */
    if (start == end)
        return true;

    /* Search for ':' on the line. */
    const char* separator = start;
    for (; separator != end; separator++)
    {
        if (*separator == ':')
            break;
    }

    /* Skip line if no ':' separator was found. */
    if (separator == end)
        return true;

    /* Skip trailing spaces in key part. */
    const char* key_end = separator;
    for (; key_end != start; key_end--)
    {
        if (key_end[-1] != ' ' && key_end[-1] != '\t')
            break;
    }

    /* Skip line if key contains nothing but spaces. */
    if (key_end == start)
        return true;

    /* Skip leading spaces in value part. */
    const char* value_start = separator + 1;
    for (; value_start != end; value_start++)
    {
        if (*value_start != ' ')
            break;
    }

    /* Value part contains nothing but spaces. Skip line. */
    if (value_start == end)
        return true;

    /* Skip trailing spaces in value part (if any) */
    const char* value_end = end;
    for (; value_end != value_start; value_end--)
    {
        if (value_end[-1] != ' ')
            break;
    }

    const size_t key_length = key_end - start;

    switch (key_length)
    {
    case 6:
        if (StrEqual(start, "Serial", key_length))
        {
            /* Usually contains just zeros, useless */
        }
        else if (StrEqual(start, "I size", key_length)) // instruction cache size
        {
            parse_cache_number(value_start, value_end,);
        }
        else if (StrEqual(start, "I sets", key_length)) // instruction cache sets
        {
            parse_cache_number(value_start, value_end);
        }
        else if (StrEqual(start, "D size", key_length)) // data cache size
        {
            parse_cache_number(value_start, value_end);
        }
        else if (StrEqual(start, "D sets", key_length)) // data cache sets
        {
            parse_cache_number(value_start, value_end);
        }
        break;
    case 7:
        if (StrEqual(start, "I assoc", key_length)) // instruction cache associativity
        {
            parse_cache_number(value_start, value_end);
        }
        else if (StrEqual(start, "D assoc", key_length)) // data cache associativity
        {
            parse_cache_number(value_start, value_end);
        }
        break;
    case 8:
        if (StrEqual(start, "CPU part", key_length))
        {
            parse_cpu_part(value_start, value_end);
        }
        else if (StrEqual(start, "Features", key_length))
        {
            parse_features(value_start, value_end);
        }
        else if (StrEqual(start, "BogoMIPS", key_length))
        {
            /* BogoMIPS is useless, don't parse */
        }
        else if (StrEqual(start, "Hardware", key_length))
        {
            size_t value_length = value_end - value_start;
            if (value_length > CPUINFO_HARDWARE_VALUE_MAX)
            {
                value_length = CPUINFO_HARDWARE_VALUE_MAX;
            }
            else
            {
                state->hardware[value_length] = '\0';
            }
            memcpy(state->hardware, value_start, value_length);
        }
        else if (StrEqual(start, "Revision", key_length))
        {
            /* Board revision, no use for now */
        }
        break;
    case 9:
        if (StrEqual(start, "processor", key_length))
        {
            const u32 new_processor_index = parse_processor_number(value_start, value_end);
            if (new_processor_index < processor_index)
            {
                /* Strange: decreasing processor number */
                cpuinfo_log_warning(
                            "unexpectedly low processor number %"PRIu32" following processor %"PRIu32" in /proc/cpuinfo",
                            new_processor_index, processor_index);
            }
            else if (new_processor_index > processor_index + 1)
            {
                /* Strange, but common: skipped processor $(processor_index + 1) */
                cpuinfo_log_info(
                            "unexpectedly high processor number %"PRIu32" following processor %"PRIu32" in /proc/cpuinfo",
                            new_processor_index, processor_index);
            }
            if (new_processor_index < max_processors_count)
            {
                /* Record that the processor was mentioned in /proc/cpuinfo */
                processors[new_processor_index].flags |= CPUINFO_ARM_LINUX_VALID_PROCESSOR;
            }
            else
            {
                /* Log and ignore processor */
                cpuinfo_log_warning("processor %"PRIu32" in /proc/cpuinfo is ignored: index exceeds system limit %"PRIu32,
                                    new_processor_index, max_processors_count - 1);
            }
            state->processor_index = new_processor_index;
            return true;
        }
        else if (StrEqual(start, "Processor", key_length))
        {
            /* TODO: parse to fix misreported architecture, similar to Android's cpufeatures */
        }
        break;
    case 11:
        if (StrEqual(start, "CPU variant", key_length))
        {
            parse_cpu_variant(value_start, value_end, processor);
        }
        break;
    case 12:
        if (StrEqual(start, "CPU revision", key_length))
        {
            parse_cpu_revision(value_start, value_end, processor);
        }
        break;
    case 13:
        if (StrEqual(start, "I line length", key_length))  // instruction cache line size
        {
            parse_cache_number(value_start, value_end);
        }
        else if (StrEqual(start, "D line length", key_length)) // data cache line size
        {
            parse_cache_number(value_start, value_end);
        }
        break;

    case 15:
        if (StrEqual(start, "CPU implementer", key_length))
        {
            parse_cpu_implementer(value_start, value_end);
        }
        else if (StrEqual(start, "CPU implementor", key_length))
        {
            parse_cpu_implementer(value_start, value_end);
        }
        break;
    case 16:
        if (StrEqual(start, "CPU architecture", key_length))
        {
            parse_cpu_architecture(value_start, value_end);
        }
        break;
    }
    return true;
}

bool cpuinfo_arm_linux_parse_proc_cpuinfo(
        char hardware[restrict static CPUINFO_HARDWARE_VALUE_MAX],
uint32_t max_processors_count,
struct cpuinfo_arm_linux_processor processors[restrict static max_processors_count])
{
    struct proc_cpuinfo_parser_state state = {
        .hardware = hardware,
                .processor_index = 0,
                .max_processors_count = max_processors_count,
                .processors = processors,
    };
    return unix_parse_multiline_file("/proc/cpuinfo", BUFFER_SIZE,
                                     (cpuinfo_line_callback) parse_line, &state);
}

void decode_vendor_uarch(midr_t midr, bool has_vfpv4, eCPUVendors *vendor, eCPUuArchs *uarch)
{
    switch (midr.implementer())
    {
    case 'A':
        *vendor = CPU_VendorARM;
        switch (midr.part())
        {
        case 0xC05:
            *uarch = CPU_uArchCortexA5;
            break;
        case 0xC07:
            *uarch = CPU_uArchCortexA7;
            break;
        case 0xC08:
            *uarch = CPU_uArchCortexA8;
            break;
        case 0xC09:
            *uarch = CPU_uArchCortexA9;
            break;
        case 0xC0C:
            *uarch = CPU_uArchCortexA12;
            break;
        case 0xC0E:
            *uarch = CPU_uArchCortexA17;
            break;
        case 0xC0D:
            /*
             * Rockchip RK3288 only.
             * Core information is ambiguous: some sources specify Cortex-A12, others - Cortex-A17.
             * Assume it is Cortex-A12.
             */
            *uarch = CPU_uArchCortexA12;
            break;
        case 0xC0F:
            *uarch = CPU_uArchCortexA15;
            break;
        case 0xD01:
            *uarch = CPU_uArchCortexA32;
            break;
        case 0xD03:
            *uarch = CPU_uArchCortexA53;
            break;
        case 0xD04:
            *uarch = CPU_uArchCortexA35;
            break;
        case 0xD05:
            *uarch = CPU_uArchCortexA55;
            break;
        case 0xD06:
            *uarch = CPU_uArchCortexA65;
            break;
        case 0xD07:
            *uarch = CPU_uArchCortexA57;
            break;
        case 0xD08:
            *uarch = CPU_uArchCortexA72;
            break;
        case 0xD09:
            *uarch = CPU_uArchCortexA73;
            break;
        case 0xD0A:
            *uarch = CPU_uArchCortexA75;
            break;
        case 0xD0B:
            *uarch = CPU_uArchCortexA76;
            break;
#   if kyArchIs64Bit && !kyOSIsAndroid
        case 0xD0C:
            *uarch = CPU_uArchNeoverseN1;
            break;
#   endif
        case 0xD0D:
            *uarch = CPU_uArchCortexA77;
            break;
        case 0xD0E:
            *uarch = CPU_uArchCortexA76AE;
            break;
#   if kyArchIs64Bit && !kyOSIsAndroid
        case 0xD4A:
            *uarch = CPU_uArchNeoverseE1;
            break;
#   endif
        default:
            switch (midr.part() >> 8)
            {
            case 7:
                *uarch = CPU_uArchARM7;
                break;
            case 9:
                *uarch = CPU_uArchARM9;
                break;
            case 11:
                *uarch = CPU_uArchARM11;
                break;
            }
        }
        break;
    case 'B':
        *vendor = CPU_VendorBroadcom;
        switch (midr.part())
        {
        case 0x00F:
            *uarch = CPU_uArchBrahmaB15;
            break;
        case 0x100:
            *uarch = CPU_uArchBrahmaB53;
            break;
#   if kyArchIs64Bit && !kyOSIsAndroid
        case 0x516:
            /* Broadcom Vulkan was sold to Cavium before it reached the market, so we identify it as Cavium ThunderX2 */
            *vendor = CPU_VendorCavium;
            *uarch = CPU_uArchThunderX2;
            break;
#   endif
        }
        break;
#   if kyArchIs64Bit && !kyOSIsAndroid
    case 'C':
        *vendor = CPU_VendorCavium;
        switch (midr.part())
        {
        case 0x0A0: /* ThunderX */
        case 0x0A1: /* ThunderX 88XX */
        case 0x0A2: /* ThunderX 81XX */
        case 0x0A3: /* ThunderX 83XX */
            *uarch = CPU_uArchThunderX;
            break;
        case 0x0AF: /* ThunderX2 99XX */
            *uarch = CPU_uArchThunderX2;
            break;
        }
        break;
#   endif
    case 'H':
        *vendor = CPU_VendorHuawei;
        switch (midr.part()) {
        case 0xD40: /* Kirin 980 Big/Medium cores -> Cortex-A76 */
            *vendor = CPU_VendorARM;
            *uarch = CPU_uArchCortexA76;
            break;
        }
        break;
    case 'i':
        *vendor = CPU_VendorIntel;
        switch (midr.part() >> 8)
        {
        case 2: /* PXA 210/25X/26X */
        case 4: /* PXA 27X */
        case 6: /* PXA 3XX */
            *uarch = CPU_uArchXScale;
            break;
        }
        break;
    case 'N':
        *vendor = CPU_VendorNvidia;
        switch (midr.part())
        {
        case 0x000:
            *uarch = CPU_uArchDenver;
            break;
        case 0x003:
            *uarch = CPU_uArchDenver2;
            break;
        case 0x004:
            *uarch = CPU_uArchCarmel;
            break;
        }
        break;
#   if !kyOSIsAndroid
    case 'P':
        *vendor = CPU_VendorAPM;
        switch (midr.part())
        {
        case 0x000:
            *uarch = CPU_uArchXGene;
            break;
        }
        break;
#   endif
    case 'Q':
        *vendor = CPU_VendorQualcomm;
        switch (midr.part())
        {
        case 0x00F:
            /* Mostly Scorpions, but some Cortex A5 may report this value as well */
            if (has_vfpv4)
            {
                /* Unlike Scorpion, Cortex-A5 comes with VFPv4 */
                *vendor = CPU_VendorARM;
                *uarch = CPU_uArchCortexA5;
            }
            else
                *uarch = CPU_uArchScorpion;
            break;
        case 0x02D: /* Dual-core Scorpions */
            *uarch = CPU_uArchScorpion;
            break;
        case 0x04D:
            /*
             * Dual-core Krait:
             * - r1p0 -> Krait 200
             * - r1p4 -> Krait 200
             * - r2p0 -> Krait 300
             */
        case 0x06F:
            /*
             * Quad-core Krait:
             * - r0p1 -> Krait 200
             * - r0p2 -> Krait 200
             * - r1p0 -> Krait 300
             * - r2p0 -> Krait 400 (Snapdragon 800 MSMxxxx)
             * - r2p1 -> Krait 400 (Snapdragon 801 MSMxxxxPRO)
             * - r3p1 -> Krait 450
             */
            *uarch = CPU_uArchKrait;
            break;
        case 0x201: /* Qualcomm Snapdragon 821: Low-power Kryo "Silver" */
        case 0x205: /* Qualcomm Snapdragon 820 & 821: High-performance Kryo "Gold" */
        case 0x211: /* Qualcomm Snapdragon 820: Low-power Kryo "Silver" */
            *uarch = CPU_uArchKryo;
            break;
        case 0x800: /* High-performance Kryo 260 (r10p2) / Kryo 280 (r10p1) "Gold" -> Cortex-A73 */
            *vendor = CPU_VendorARM;
            *uarch = CPU_uArchCortexA73;
            break;
        case 0x801: /* Low-power Kryo 260 / 280 "Silver" -> Cortex-A53 */
            *vendor = CPU_VendorARM;
            *uarch = CPU_uArchCortexA53;
            break;
        case 0x802: /* High-performance Kryo 385 "Gold" -> Cortex-A75 */
            *vendor = CPU_VendorARM;
            *uarch = CPU_uArchCortexA75;
            break;
        case 0x803: /* Low-power Kryo 385 "Silver" -> Cortex-A55 */
            *vendor = CPU_VendorARM;
            *uarch = CPU_uArchCortexA55;
            break;
        case 0x804: /* High-performance Kryo 485 "Gold" / "Gold Prime" -> Cortex-A76 */
            *vendor = CPU_VendorARM;
            *uarch = CPU_uArchCortexA76;
            break;
        case 0x805: /* Low-performance Kryo 485 "Silver" -> Cortex-A55 */
            *vendor = CPU_VendorARM;
            *uarch = CPU_uArchCortexA55;
            break;
#   if kyArchIs64Bit && !kyOSIsAndroid
        case 0xC00:
            *uarch = CPU_uArchFalkor;
            break;
        case 0xC01:
            *uarch = CPU_uArchSaphira;
            break;
#   endif
        }
        break;
    case 'S':
        *vendor = CPU_VendorSamsung;
        switch ((midr.part() << midrFlags(part)) | (midr.variant() << midrFlags(variant)))
        {
        case 0x00100010:
            /*
             * Exynos 8890 MIDR = 0x531F0011, assume Exynos M1 has:
             * - CPU variant 0x1
             * - CPU part 0x001
             */
            *uarch = CPU_uArchExynosM1;
            break;
        case 0x00400010:
            /*
             * Exynos 8895 MIDR = 0x534F0010, assume Exynos M2 has:
             * - CPU variant 0x4
             * - CPU part 0x001
             */
            *uarch = CPU_uArchExynosM2;
            break;
        case 0x00100020:
            /*
             * Exynos 9810 MIDR = 0x531F0020, assume Exynos M3 has:
             * - CPU variant 0x1
             * - CPU part 0x002
             */
            *uarch = CPU_uArchExynosM3;
            break;
        case 0x00100030:
            /*
             * Exynos 9820 MIDR = 0x531F0030, assume Exynos M4 has:
             * - CPU variant 0x1
             * - CPU part 0x003
             */
            *uarch = CPU_uArchExynosM4;
            break;
        case 0x00100040:
            /*
             * Exynos 9820 MIDR = 0x531F0040, assume Exynos M5 has:
             * - CPU variant 0x1
             * - CPU part 0x004
             */
            *uarch = CPU_uArchExynosM5;
            break;
        }
        break;
    case 'V':
        *vendor = CPU_VendorMarvell;
        switch (midr.part())
        {
        case 0x581: /* PJ4 / PJ4B */
        case 0x584: /* PJ4B-MP / PJ4C */
            *uarch = CPU_uArchPJ4;
            break;
        }
        break;
    }
}

#endif

