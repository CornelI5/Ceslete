#ifndef CESLETE_H
#define CESLETE_H

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

struct FramebufferInfo {
    u64 base_address;
    u32 width;
    u32 height;
    u32 pitch;
    u32 pixel_format;
};

struct MemoryRegion {
    u64 physical_start;
    u64 page_count;
    u32 type;
    u32 attribute;
};

struct ToastHandoff {
    struct FramebufferInfo fb;
    struct MemoryRegion *mmap;
    u64 mmap_size;
    u64 mmap_desc_size;
    u64 rsdp_address;
    u64 system_table;
    u32 toast_version;
    u32 flags;
};

#define TOAST_HANDOFF_MAGIC 0xCE57E7E
#define TOAST_HANDOFF_VERSION 1

#endif
