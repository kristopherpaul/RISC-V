#ifndef DRAM_H
#define DRAM_H

#include "utils.h"

typedef struct DRAM{
    u8 mem[DRAM_SIZE];
} DRAM;

extern DRAM dram;

u64 load(u64 addr, u64 size);

void store(u64 addr, u64 size, u64 val);

#endif