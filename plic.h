#ifndef PLIC_H
#define PLIC_H

#include "utils.h"

typedef struct PLIC {
    u64 pending;
    u64 senable;
    u64 spriority;
    u64 sclaim;
} PLIC;

extern PLIC plic;

u64 plic_load32(u64 addr);

void plic_store32(u64 addr, u64 value);

u64 plic_load(u64 addr, u64 size);

int plic_store(u64 addr, u64 size, u64 value);

#endif
