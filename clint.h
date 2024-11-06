#ifndef CLINT_H
#define CLINT_H

#include "utils.h"

typedef struct CLINT {
    u64 mtime;
    u64 mtimecmp;
} CLINT;

extern CLINT clint;

u64 clint_load(u64 addr, u64 size, Exception* exception);

void clint_store(u64 addr, u64 size, u64 value, Exception* exception);

u64 clint_load64(u64 addr);

void clint_store64(u64 addr, u64 value);
#endif
