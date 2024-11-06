///Code 2

#include <stdint.h>
#include <utils.h>

typedef struct {
    u64 pending;
    u64 senable;
    u64 spriority;
    u64 sclaim;
} Plic;

u64 plic_load32(Plic* plic, u64 addr) {
    switch (addr) {
        case PLIC_PENDING:
            return plic->pending;
        case PLIC_SENABLE:
            return plic->senable;
        case PLIC_SPRIORITY:
            return plic->spriority;
        case PLIC_SCLAIM:
            return plic->sclaim;
        default:
            return 0;
    }
}

void plic_store32(Plic* plic, u64 addr, u64 value) {
    switch (addr) {
        case PLIC_PENDING:
            plic->pending = value;
            break;
        case PLIC_SENABLE:
            plic->senable = value;
            break;
        case PLIC_SPRIORITY:
            plic->spriority = value;
            break;
        case PLIC_SCLAIM:
            plic->sclaim = value;
            break;
        default:
            break;
    }
}

Plic* plic_new() {
    Plic* plic = (Plic*)malloc(sizeof(Plic));
    plic->pending = 0;
    plic->senable = 0;
    plic->spriority = 0;
    plic->sclaim = 0;
    return plic;
}

u64 plic_load(Plic* plic, u64 addr, u64 size) {
    if (size == 32) {
        return plic_load32(plic, addr);
    } else {
        return LoadAccessFault;
    }
}

int plic_store(Plic* plic, u64 addr, u64 size, u64 value) {
    if (size == 32) {
        plic_store32(plic, addr, value);
        return 0;
    } else {
        return StoreAMOAccessFault;
    }
}