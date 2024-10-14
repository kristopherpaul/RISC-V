#include "dram.h"

DRAM dram;

u64 load(u64 addr, u64 size){
    int index = addr - DRAM_BASE;
    u64 val = 0;
    for(int i = 0;i < (size/8);i++){
        val |= (u64)dram.mem[index+i] << (i*8);
    }
    return val;
}

void store(u64 addr, u64 size, u64 val){
    int index = addr - DRAM_BASE;
    for(int i = 0;i < (size/8);i++){
        dram.mem[index+i] = (val >> (i*8)) & 0xff;
    }
}