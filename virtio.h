#ifndef VIRTIO_H
#define VIRTIO_H

#include "utils.h"

typedef struct VIRTIO{
    u64 id;
    u32 driver_features;
    u32 page_size;
    u32 queue_sel;
    u32 queue_num;
    u32 queue_pfn;
    u32 queue_notify;
    u32 status;
    u8 disk[VIRTIO_SIZE];
} VIRTIO;

extern VIRTIO virtio;

void initVIRTIO();

bool is_virtio_interrupting();

Result load_virtio(u64 addr, u64 size);

Result store_virtio(u64 addr, u64 size, u64 val);

void virtio_disk_access();

#endif