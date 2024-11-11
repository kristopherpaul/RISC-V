#ifndef PLIC_H
#define PLIC_H

#include "utils.h"

typedef struct PLIC{
    u64 pending;
    u64 senable;
    u64 spriority;
    u64 sclaim;
} PLIC;

extern PLIC plic;

Result load_plic(u64 addr, u64 size);

Result store_plic(u64 addr, u64 size, u64 val);

#endif