
#ifndef X86_CACHE_TABLE_H
#define X86_CACHE_TABLE_H


#define FA 0xff
#define CACHE ky_cpu::cache_t
#define X86C(C, S, A, E, F) \
{(C), (S), (A), (E), F | CACHE::Static, 1, 1, 0}
struct arch_cache
{
    const uint8  code;
    const uint   size;     // TLB:pages, cache:line size
    const uint8  associate;// -way
    const uint16 entries;  // entries
    const uint16 flag;     // level, type
    const uint8  sharedBy;
    const uint8  core;
    void *       next;
};

static const arch_cache x86_cache_table[] =
{
X86C ( 0x01, 04*kyKiB, 04, 32, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K pages, 4-way, 32 entries"},
X86C ( 0x02, 04*kyMiB, 04, 02, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4M pages, 4-way, 2 entries"},
X86C ( 0x03, 04*kyKiB, 04, 64, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, 4-way, 64 entries"},
X86C ( 0x04, 04*kyMiB, 04, 08, CACHE::Data |        CACHE::TLB), // "data TLB: 4M pages, 4-way, 8 entries"},
X86C ( 0x05, 04*kyMiB, 04, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 4M pages, 4-way, 32 entries"},
X86C ( 0x06, 08*kyKiB, 04, 32, CACHE::Instruction | CACHE::L1 ), // "L1 instruction cache: 8K, 4-way, 32 byte lines"},
X86C ( 0x08, 16*kyKiB, 04, 32, CACHE::Instruction | CACHE::L1 ), // "L1 instruction cache: 16K, 4-way, 32 byte lines"},
X86C ( 0x09, 32*kyKiB, 04, 64, CACHE::Instruction | CACHE::L1 ), // "L1 instruction cache: 32K, 4-way, 64-byte lines"},
X86C ( 0x0a, 08*kyKiB, 02, 32, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 8K, 2-way, 32 byte lines"},
X86C ( 0x0b, 04*kyMiB, 04, 04, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4M pages, 4-way, 4 entries"},
X86C ( 0x0c, 16*kyKiB, 04, 32, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 16K, 4-way, 32 byte lines"},
X86C ( 0x0d, 16*kyKiB, 04, 64, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 16K, 4-way, 64-byte lines"},
X86C ( 0x0e, 24*kyKiB, 06, 64, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 24K, 6-way, 64 byte lines"},
X86C ( 0x10, 16*kyKiB, 04, 32, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 16K, 4-way, 32 byte lines"},
X86C ( 0x15, 16*kyKiB, 04, 32, CACHE::Instruction | CACHE::L1 ), // "L1 instruction cache: 16K, 4-way, 32 byte lines"},
X86C ( 0x1d,128*kyKiB, 02, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 128K, 2-way, 64 byte lines"},
X86C ( 0x1a, 96*kyKiB, 06, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 96K, 6-way, 64 byte lines"},
X86C ( 0x21,256*kyKiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 256K MLC, 8-way, 64 byte lines"},
X86C ( 0x22,512*kyKiB, 04, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 512K, 4-way, 64 byte lines"},
X86C ( 0x23, 01*kyMiB, 08, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 1M, 8-way, 64 byte lines"},
X86C ( 0x24, 01*kyMiB, 16, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 1M, 16-way, 64 byte lines"},
X86C ( 0x25, 02*kyMiB, 08, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 2M, 8-way, 64 byte lines"},
X86C ( 0x29, 04*kyMiB, 08, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 4M, 8-way, 64 byte lines"},
X86C ( 0x2c, 32*kyKiB, 08, 64, CACHE::Data    |     CACHE::L1 ), // "L1 data cache: 32K, 8-way, 64 byte lines"},
X86C ( 0x30, 32*kyKiB, 08, 64, CACHE::Unified |     CACHE::L1 ), // "L1 cache: 32K, 8-way, 64 byte lines"},
X86C ( 0x39,128*kyKiB, 04, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 128K, 4-way, sectored, 64 byte lines"},
X86C ( 0x3a,192*kyKiB,  6, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 192K, 6-way, sectored, 64 byte lines"
X86C ( 0x3b,128*kyKiB,  2, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 128K, 2-way, sectored, 64 byte lines"
X86C ( 0x3c,256*kyKiB,  4, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 256K, 4-way, sectored, 64 byte lines"
X86C ( 0x3d,384*kyKiB,  6, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 384K, 6-way, sectored, 64 byte lines"
X86C ( 0x3e,512*kyKiB,  4, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 512K, 4-way, sectored, 64 byte lines"
X86C ( 0x41,128*kyKiB,  4, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 128K, 4-way, 32 byte lines"
X86C ( 0x42,256*kyKiB,  4, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 256K, 4-way, 32 byte lines"
X86C ( 0x43,512*kyKiB,  4, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 512K, 4-way, 32 byte lines"
X86C ( 0x44, 01*kyMiB,  4, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 1M, 4-way, 32 byte lines"
X86C ( 0x45, 02*kyMiB,  4, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 2M, 4-way, 32 byte lines"
X86C ( 0x46, 04*kyMiB,  4, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 4M, 4-way, 64 byte lines"
X86C ( 0x47, 08*kyMiB,  8, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 8M, 8-way, 64 byte lines"
X86C ( 0x48, 03*kyMiB, 12, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 3M, 12-way, 64 byte lines"
X86C ( 0x49, 04*kyMiB, 16, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 4M, 16-way, 64 byte lines"

X86C ( 0x4a, 06*kyMiB, 12, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 6M, 12-way, 64 byte lines"
X86C ( 0x4b, 08*kyMiB, 16, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 8M, 16-way, 64 byte lines"
X86C ( 0x4c, 12*kyMiB, 12, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 12M, 12-way, 64 byte lines"
X86C ( 0x4d, 16*kyMiB, 16, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 16M, 16-way, 64 byte lines"
X86C ( 0x4e, 06*kyMiB, 24, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 6M, 24-way, 64 byte lines"
X86C ( 0x4f, 04*kyKiB, FA, 32, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K pages, 32 entries"
X86C ( 0x50, 04*kyKiB, FA, 64, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 64 entries"
X86C ( 0x50, 02*kyMiB, FA, 64, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 64 entries"
X86C ( 0x50, 04*kyMiB, FA, 64, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 64 entries"
X86C ( 0x51, 04*kyKiB, FA,128, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 128 entries"
X86C ( 0x51, 02*kyMiB, FA,128, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 128 entries"
X86C ( 0x51, 04*kyMiB, FA,128, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 128 entries"
X86C ( 0x52, 04*kyKiB, FA,256, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 256 entries"
X86C ( 0x52, 02*kyMiB, FA,256, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 256 entries"
X86C ( 0x52, 04*kyMiB, FA,256, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K & 2M/4M pages, 256 entries"
X86C ( 0x55, 02*kyMiB, FA, 07, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 2M/4M pages, fully, 7 entries"
X86C ( 0x55, 04*kyMiB, FA, 07, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 2M/4M pages, fully, 7 entries"
X86C ( 0x56, 04*kyMiB, 04, 16,CACHE::Data|CACHE::L1|CACHE::TLB), // "L1 data TLB: 4M pages, 4-way, 16 entries"
X86C ( 0x57, 04*kyKiB, 04, 16,CACHE::Data|CACHE::L1|CACHE::TLB), // "L1 data TLB: 4K pages, 4-way, 16 entries"
X86C ( 0x59, 04*kyKiB, FA, 16, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, 16 entries"
X86C ( 0x5a, 04*kyMiB, 04, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 4-way, 32 entries"
X86C ( 0x5a, 02*kyMiB, 04, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 4-way, 32 entries"
X86C ( 0x5b, 04*kyKiB, FA, 64, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 64 entries"
X86C ( 0x5b, 04*kyMiB, FA, 64, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 64 entries"
X86C ( 0x5c, 04*kyKiB, FA,128, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 128 entries"
X86C ( 0x5c, 04*kyMiB, FA,128, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 128 entries"
X86C ( 0x5d, 04*kyKiB, FA,256, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 256 entries"
X86C ( 0x5d, 04*kyMiB, FA,256, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 256 entries"
X86C ( 0x60, 16*kyKiB, 08, 64, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 16K, 8-way, 64 byte lines"
X86C ( 0x61, 04*kyKiB, FA, 48, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K pages, 48 entries"
X86C ( 0x63, 02*kyMiB, 04, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 4-way, 32 entries"
X86C ( 0x63, 04*kyMiB, 04, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 4-way, 32 entries"
X86C ( 0x63, 01*kyGiB, 04, 04, CACHE::Data |        CACHE::TLB), // "data TLB: 1G pages, 4-way, 4 entries"
X86C ( 0x64, 04*kyKiB, 04,512, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, 4-way, 512 entries"
X86C ( 0x66, 08*kyKiB, 04, 64, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 8K, 4-way, 64 byte lines"
X86C ( 0x67, 16*kyKiB, 04, 64, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 16K, 4-way, 64 byte lines"
X86C ( 0x68, 32*kyKiB, 04, 64, CACHE::Data |        CACHE::L1 ), // "L1 data cache: 32K, 4-way, 64 byte lines"
X86C ( 0x6a, 04*kyKiB, 08, 64, CACHE::Data |        CACHE::TLB), // "micro-data TLB: 4K pages, 8-way, 64 entries"
X86C ( 0x6b, 04*kyKiB, 08,256, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, 8-way, 256 entries"
X86C ( 0x6c, 02*kyMiB, 08,128, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 8-way, 128 entries"
X86C ( 0x6c, 04*kyMiB, 08,128, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 8-way, 128 entries"
X86C ( 0x6d, 01*kyGiB, FA, 16, CACHE::Data |        CACHE::TLB), // "data TLB: 1G pages, fully, 16 entries"
X86C ( 0x70, 12*kyKiB, 08, 01, CACHE::Trace                   ), // "Trace cache: 12K-uop, 8-way"
X86C ( 0x71, 16*kyKiB, 08, 01, CACHE::Trace                   ), // "Trace cache: 16K-uop, 8-way"
X86C ( 0x72, 32*kyKiB, 08, 01, CACHE::Trace                   ), // "Trace cache: 32K-uop, 8-way"
X86C ( 0x73, 64*kyKiB, 08, 01, CACHE::Trace                   ), // "Trace cache: 64K-uop, 8-way"
X86C ( 0x76, 02*kyMiB, FA, 08, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 2M/4M pages, fully, 8 entries"
X86C ( 0x76, 04*kyMiB, FA, 08, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 2M/4M pages, fully, 8 entries"
X86C ( 0x77, 16*kyKiB, 04, 64, CACHE::Instruction | CACHE::L1 ), // "L1 instruction cache: 16K, 4-way, sectored, 64 byte lines"

X86C ( 0x78, 01*kyMiB, 04, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 1M, 4-way, 64 byte lines"
X86C ( 0x79,128*kyKiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 128K, 8-way, sectored, 64 byte lines"
X86C ( 0x7a,256*kyKiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 256K, 8-way, sectored, 64 byte lines"
X86C ( 0x7b,512*kyKiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 512K, 8-way, sectored, 64 byte lines"
X86C ( 0x7c, 01*kyMiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 1M, 8-way, sectored, 64 byte lines"
X86C ( 0x7d, 02*kyMiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 2M, 8-way, 64 byte lines"
X86C ( 0x7e,256*kyKiB, 08,128, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 256K, 8-way, sectored, 128 byte lines"
X86C ( 0x7f,512*kyKiB, 02, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 512K, 2-way, 64 byte lines"
X86C ( 0x80,512*kyKiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 512K, 8-way, 64 byte lines"
X86C ( 0x81,128*kyKiB, 08, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 128K, 8-way, 32 byte lines"
X86C ( 0x82,256*kyKiB, 08, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 256K, 8-way, 32 byte lines"
X86C ( 0x83,512*kyKiB, 08, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 512K, 8-way, 32 byte lines"
X86C ( 0x84, 01*kyMiB, 08, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 1M, 8-way, 32 byte lines"
X86C ( 0x85, 02*kyMiB, 08, 32, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 2M, 8-way, 32 byte lines"
X86C ( 0x86,512*kyKiB, 04, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 512K, 4-way, 64 byte lines"
X86C ( 0x87, 01*kyMiB, 08, 64, CACHE::Unified |     CACHE::L2 ), // "L2 cache: 1M, 8-way, 64 byte lines"
X86C ( 0x88, 02*kyMiB, 04, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 2M, 4-way, 64 byte lines"
X86C ( 0x89, 04*kyMiB, 04, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 4M, 4-way, 64 byte lines"
X86C ( 0x8a, 08*kyMiB, 04, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 8M, 4-way, 64 byte lines"
X86C ( 0x8d, 03*kyMiB, 12,128, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 3M, 12-way, 128 byte lines"
X86C ( 0x90, 04*kyKiB, FA, 64, CACHE::Instruction | CACHE::TLB | CACHE::Range), // "instruction TLB: 4K-256M, fully, 64 entries"
X86C ( 0x90,256*kyMiB, FA, 64, CACHE::Instruction | CACHE::TLB | CACHE::Range), // "instruction TLB: 4K-256M, fully, 64 entries"
X86C ( 0x96, 04*kyKiB, FA, 32, CACHE::Instruction | CACHE::TLB | CACHE::Range), // "instruction TLB: 4K-256M, fully, 32 entries"
X86C ( 0x96,256*kyMiB, FA, 32, CACHE::Instruction | CACHE::TLB | CACHE::Range), // "instruction TLB: 4K-256M, fully, 32 entries"
X86C ( 0x9b, 04*kyKiB, FA, 96, CACHE::Instruction | CACHE::TLB | CACHE::Range), // "instruction TLB: 4K-256M, fully, 96 entries"
X86C ( 0x9b,256*kyMiB, FA, 96, CACHE::Instruction | CACHE::TLB | CACHE::Range), // "instruction TLB: 4K-256M, fully, 96 entries"
X86C ( 0xa0, 04*kyKiB, FA, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, fully, 32 entries"
X86C ( 0xb0, 04*kyKiB, 04,128, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K, 4-way, 128 entries"
X86C ( 0xb1, 02*kyMiB, 04, 08, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 2M/4M, 4-way, 4/8 entries"
X86C ( 0xb1, 04*kyMiB, 04, 04, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 2M/4M, 4-way, 4/8 entries"

X86C ( 0xb2, 04*kyKiB, 04, 64, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K, 4-way, 64 entries"
X86C ( 0xb3, 04*kyKiB, 04,128, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, 4-way, 128 entries"
X86C ( 0xb4, 04*kyKiB, 04,256, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, 4-way, 256 entries"
X86C ( 0xb5, 04*kyKiB, 08, 64, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K, 8-way, 64 entries"
X86C ( 0xb6, 04*kyKiB, 08,128, CACHE::Instruction | CACHE::TLB), // "instruction TLB: 4K, 8-way, 128 entries"
X86C ( 0xba, 04*kyKiB, 04, 64, CACHE::Data |        CACHE::TLB), // "data TLB: 4K pages, 4-way, 64 entries"
X86C ( 0xc0, 04*kyKiB, 04, 08, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 4-way, 8 entries"
X86C ( 0xc0, 04*kyMiB, 04, 08, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 4M pages, 4-way, 8 entries"
X86C ( 0xc1, 04*kyKiB, 08,1024,CACHE::L2   |        CACHE::TLB), // "L2 TLB: 4K/2M pages, 8-way, 1024 entries"
X86C ( 0xc1, 02*kyMiB, 08,1024,CACHE::L2   |        CACHE::TLB), // "L2 TLB: 4K/2M pages, 8-way, 1024 entries"
X86C ( 0xc2, 04*kyKiB, 04, 16, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 2M pages, 4-way, 16 entries"
X86C ( 0xc2, 02*kyMiB, 04, 16, CACHE::Data |        CACHE::TLB), // "data TLB: 4K & 2M pages, 4-way, 16 entries"
X86C ( 0xc3, 04*kyKiB, 06,1536,CACHE::L2   |        CACHE::TLB), // "L2 TLB: 4K/2M pages, 6-way, 1536 entries"
X86C ( 0xc3, 02*kyMiB, 06,1536,CACHE::L2   |        CACHE::TLB), // "L2 TLB: 4K/2M pages, 6-way, 1536 entries"
X86C ( 0xc4, 02*kyMiB, 04, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 4-way, 32 entries"
X86C ( 0xc4, 04*kyMiB, 04, 32, CACHE::Data |        CACHE::TLB), // "data TLB: 2M/4M pages, 4-way, 32 entries"
X86C ( 0xca, 04*kyKiB, 04,512, CACHE::L2   |        CACHE::TLB), // "L2 TLB: 4K pages, 4-way, 512 entries"
X86C ( 0xd0,512*kyKiB, 04, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 512K, 4-way, 64 byte lines"
X86C ( 0xd1, 01*kyMiB, 04, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 1M, 4-way, 64 byte lines"
X86C ( 0xd2, 02*kyMiB, 04, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 2M, 4-way, 64 byte lines"
X86C ( 0xd6, 01*kyMiB, 08, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 1M, 8-way, 64 byte lines"
X86C ( 0xd7, 02*kyMiB, 08, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 2M, 8-way, 64 byte lines"
X86C ( 0xd8, 04*kyMiB, 08, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 4M, 8-way, 64 byte lines"
X86C ( 0xdc,1.5*kyMiB, 12, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 1.5M, 12-way, 64 byte lines"
X86C ( 0xdd, 03*kyMiB, 12, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 3M, 12-way, 64 byte lines"
X86C ( 0xde, 06*kyMiB, 12, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 6M, 12-way, 64 byte lines"
X86C ( 0xe2, 02*kyMiB, 16, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 2M, 16-way, 64 byte lines"
X86C ( 0xe3, 04*kyMiB, 16, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 4M, 16-way, 64 byte lines"
X86C ( 0xe4, 08*kyMiB, 16, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 8M, 16-way, 64 byte lines"
X86C ( 0xea, 12*kyMiB, 24, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 12M, 24-way, 64 byte lines"
X86C ( 0xeb, 18*kyMiB, 24, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 18M, 24-way, 64 byte lines"
X86C ( 0xec, 24*kyMiB, 24, 64, CACHE::Unified |     CACHE::L3 ), // "L3 cache: 24M, 24-way, 64 byte lines"
X86C ( 0xf0, 64      , 64, 64, CACHE::Prefetch                ), // "64 byte prefetching"
X86C ( 0xf1, 128     ,128,128, CACHE::Prefetch                ), // "128 byte prefetching"
X86C ( 0xfe, 0x18   ,0x18,0x18,CACHE::CPUID   |     CACHE::TLB), // "TLB data is in CPUID leaf 0x18"},
X86C ( 0xff, 0x04   ,0x04,0x04,CACHE::CPUID   |    CACHE::Data)  // "cache data is in CPUID leaf 4"}
};


#endif
