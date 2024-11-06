///Code 2

#include <plic.h>
PLIC plic;

u64 plic_load32(u64 addr) {
    switch (addr) {
        case PLIC_PENDING:
            return plic.pending;
        case PLIC_SENABLE:
            return plic.senable;
        case PLIC_SPRIORITY:
            return plic.spriority;
        case PLIC_SCLAIM:
            return plic.sclaim;
        default:
            return 0;
    }
}

void plic_store32(u64 addr, u64 value) {
    switch (addr) {
        case PLIC_PENDING:
            plic.pending = value;
            break;
        case PLIC_SENABLE:
            plic.senable = value;
            break;
        case PLIC_SPRIORITY:
            plic.spriority = value;
            break;
        case PLIC_SCLAIM:
            plic.sclaim = value;
            break;
        default:
            break;
    }
}

u64 plic_load(u64 addr, u64 size) {
    if (size == 32) {
        return plic_load32( addr);
    } else {
        return LoadAccessFault;
    }
}

int plic_store(u64 addr, u64 size, u64 value) {
    if (size == 32) {
        plic_store32(addr, value);
        return 0;
    } else {
        return StoreAMOAccessFault;
    }
}
