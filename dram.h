#ifndef DRAM_H
#define DRAM_H

#include "utils.h"

typedef struct DRAM{
    u32 mem[1024*1024];
} DRAM;

extern DRAM dram;

#endif