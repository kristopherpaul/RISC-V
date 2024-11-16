#ifndef CLINT_H
#define CLINT_H

#include "utils.h"

typedef struct CLINT{
    u64 mtime;
    u64 mtimecmp;
} CLINT;

extern CLINT clint;

Result load_clint(u64 addr, u64 size);

Result store_clint(u64 addr, u64 size, u64 val);

#endif