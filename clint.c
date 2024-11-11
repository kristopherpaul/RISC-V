#include "clint.h"

CLINT clint;

Result load_clint(u64 addr, u64 size){
    Result ret;
    ret.exception = Null;
    if(size != 64){
        ret.exception = LoadAccessFault;
        return ret;
    }
    u64 val;
    switch(addr){
        case CLINT_MTIMECMP:
            val = clint.mtimecmp;
            break;
        case CLINT_MTIME:
            val = clint.mtime;
            break;
        default:
            val = 0;    
    }
    ret.value = val;
    return ret;
}

Result store_clint(u64 addr, u64 size, u64 val){
    Result ret;
    ret.exception = Null;
    if(size != 64){
        ret.exception = StoreAMOAccessFault;
        return ret;
    }
    switch(addr){
        case CLINT_MTIMECMP:
            clint.mtimecmp = val;
            break;
        case CLINT_MTIME:
            clint.mtime = val;
            break;
    }
    return ret;
}