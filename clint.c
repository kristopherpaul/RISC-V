#include <clint.h>

CLINT clint;

u64 clint_load(u64 addr, u64 size, Exception* exception) {
    if (size == 64) {
        return clint_load64(addr);
    } else {
        *exception = LoadAccessFault;
        return 0;
    }
}

void clint_store(u64 addr, u64 size, u64 value, Exception* exception) {
    if (size == 64) {
        clint_store64(addr, value);
    } else {
        *exception = StoreAMOAccessFault;
    }
}

u64 clint_load64(u64 addr) {
    switch (addr) {
        case CLINT_MTIMECMP:
            return clint.mtimecmp;
        case CLINT_MTIME:
            return clint.mtime;
        default:
            return 0;
    }
}

void clint_store64(u64 addr, u64 value) {
    switch (addr) {
        case CLINT_MTIMECMP:
            clint.mtimecmp = value;
            break;
        case CLINT_MTIME:
            clint.mtime = value;
            break;
        default:
            break;
    }
}
