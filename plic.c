#include "plic.h"

PLIC plic;

Result load_plic(u64 addr, u64 size){
    Result ret;
    ret.exception = NullException;
    if(size != 32){
        ret.exception = LoadAccessFault;
        return ret;
    }
    u64 val;
    switch(addr){
        case PLIC_PENDING:
            val = plic.pending;
            break;
        case PLIC_SENABLE:
            val = plic.senable;
            break;
        case PLIC_SPRIORITY:
            val = plic.spriority;
            break;
        case PLIC_SCLAIM:
            val = plic.sclaim;
            break;    
        default:
            val = 0;        
    }
    ret.value = val;
    return ret;
}

Result store_plic(u64 addr, u64 size, u64 val){
    Result ret;
    ret.exception = NullException;
    if(size != 32){
        ret.exception = StoreAMOAccessFault;
        return ret;
    }
    switch(addr){
        case PLIC_PENDING:
            plic.pending = val;
            break;
        case PLIC_SENABLE:
            plic.senable = val;
            break;
        case PLIC_SPRIORITY:
            plic.spriority = val;
            break;
        case PLIC_SCLAIM:
            plic.sclaim = val;
            break;
    }
    return ret;
}