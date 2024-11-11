#ifndef DRAM_H
#define DRAM_H

#include "utils.h"
#include "plic.h"
#include "clint.h"

typedef struct DRAM{
    u8 mem[DRAM_SIZE];
} DRAM;

extern DRAM dram;

Result load(u64 addr, u64 size);

Result store(u64 addr, u64 size, u64 val);

#endif