#include "dram.h"

DRAM dram;

Result load_dram(u64 addr, u64 size){
    Result ret;
    ret.exception = NullException;
    if(size != 8 && size != 16 && size != 32 && size != 64) {
        ret.exception = LoadAccessFault;
        return ret;
    }
    u64 index = addr - DRAM_BASE;
    u64 val = 0;
    for(int i = 0;i < (size/8);i++){
        val |= (((u64)dram.mem[index+i]) << (i*8));
    }
    ret.value = val;
    return ret;
}

Result store_dram(u64 addr, u64 size, u64 val){
    Result ret;
    ret.exception = NullException;
    if(size != 8 && size != 16 && size != 32 && size != 64){
        ret.exception = StoreAMOAccessFault;
        return ret;
    }
    u64 index = addr - DRAM_BASE;
    for(int i = 0;i < (size/8);i++){
        dram.mem[index+i] = (u8) ((val >> (i*8)) & 0xff);
    }
    return ret;
}