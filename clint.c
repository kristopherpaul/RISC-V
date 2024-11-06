#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


// typedef enum {
//     LoadAccessFault,
//     StoreAMOAccessFault,
//     // Add other exceptions as needed
// } Exception;


typedef struct {
    u64 mtime;
    u64 mtimecmp;
} Clint;

Clint* clint_new() {
    Clint* clint = (Clint*)malloc(sizeof(Clint));
    if (clint) {
        clint->mtime = 0;
        clint->mtimecmp = 0;
    }
    return clint;
}

u64 clint_load(Clint* clint, u64 addr, u64 size, Exception* exception) {
    if (size == 64) {
        return clint_load64(clint, addr);
    } else {
        *exception = LoadAccessFault;
        return 0;
    }
}

void clint_store(Clint* clint, u64 addr, u64 size, u64 value, Exception* exception) {
    if (size == 64) {
        clint_store64(clint, addr, value);
    } else {
        *exception = StoreAMOAccessFault;
    }
}

u64 clint_load64(Clint* clint, u64 addr) {
    switch (addr) {
        case CLINT_MTIMECMP:
            return clint->mtimecmp;
        case CLINT_MTIME:
            return clint->mtime;
        default:
            return 0;
    }
}

void clint_store64(Clint* clint, u64 addr, u64 value) {
    switch (addr) {
        case CLINT_MTIMECMP:
            clint->mtimecmp = value;
            break;
        case CLINT_MTIME:
            clint->mtime = value;
            break;
        default:
            break;
    }
}
