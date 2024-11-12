#include "dram.h"

DRAM dram;

Result load(u64 addr, u64 size){
    Result ret;
    ret.exception = Null;
    if(CLINT_BASE <= addr && addr < CLINT_BASE+CLINT_SIZE){
        return load_clint(addr, size);
    }else if(PLIC_BASE <= addr && addr < PLIC_BASE+PLIC_SIZE){
        return load_plic(addr, size);
    }else if(UART_BASE <= addr && addr < UART_BASE+UART_SIZE){
        return load_uart(addr, size);
    }else if(addr < DRAM_BASE){
        ret.exception = LoadAccessFault;
    }
    if(size != 8 && size != 16 && size != 32 && size != 64){
        ret.exception = LoadAccessFault;
    }
    if(ret.exception != Null){
        return ret;
    }
    int index = addr - DRAM_BASE;
    u64 val = 0;
    for(int i = 0;i < (size/8);i++){
        val |= (((u64)dram.mem[index+i]) << (i*8));
    }
    ret.value = val;
    return ret;
}

Result store(u64 addr, u64 size, u64 val){
    Result ret;
    ret.exception = Null;
    if(CLINT_BASE <= addr && addr < CLINT_BASE+CLINT_SIZE){
        return store_clint(addr, size, val);
    }else if(PLIC_BASE <= addr && addr < PLIC_BASE+PLIC_SIZE){
        return store_plic(addr, size, val);
    }else if(UART_BASE <= addr && addr < UART_BASE+UART_SIZE){
        return store_uart(addr, size, val);
    }else if(addr < DRAM_BASE){
        ret.exception = StoreAMOAccessFault;
    }
    if(size != 8 && size != 16 && size != 32 && size != 64){
        ret.exception = StoreAMOAccessFault;
    }
    if(ret.exception != Null){
        return ret;
    }
    int index = addr - DRAM_BASE;
    for(int i = 0;i < (size/8);i++){
        dram.mem[index+i] = (u8) ((val >> (i*8)) & 0xff);
    }
    return ret;
}