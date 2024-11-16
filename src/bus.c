#include "bus.h"

Result tload(u64 addr, u64 size, Access access) {
    Result taddr = translate(addr, access);
    if(taddr.exception != NullException)
        return taddr;
    else
        return load(taddr.value, size);
}

Result tstore(u64 addr, u64 size, u64 val, Access access) {
    Result taddr = translate(addr, access);
    if(taddr.exception != NullException)
        return taddr;
    else
        return store(taddr.value, size, val);
}

Result load(u64 addr, u64 size) {
    if(CLINT_BASE <= addr && addr < CLINT_BASE+CLINT_SIZE)
        return load_clint(addr, size);
    else if(PLIC_BASE <= addr && addr < PLIC_BASE+PLIC_SIZE)
        return load_plic(addr, size);
    else if(UART_BASE <= addr && addr < UART_BASE+UART_SIZE)
        return load_uart(addr, size);
    else if(DRAM_BASE <= addr && addr < DRAM_BASE+DRAM_SIZE)
        return load_dram(addr, size);
    else if(VIRTIO_BASE <= addr && addr < VIRTIO_BASE+VIRTIO_SIZE)
        return load_virtio(addr, size);
    else
        return (Result) {.exception = LoadAccessFault, .value = 0};
}

Result store(u64 addr, u64 size, u64 val) {
    if(CLINT_BASE <= addr && addr < CLINT_BASE+CLINT_SIZE)
        return store_clint(addr, size, val);
    else if(PLIC_BASE <= addr && addr < PLIC_BASE+PLIC_SIZE)
        return store_plic(addr, size, val);
    else if(UART_BASE <= addr && addr < UART_BASE+UART_SIZE)
        return store_uart(addr, size, val);
    else if(DRAM_BASE <= addr && addr < DRAM_BASE+DRAM_SIZE)
        return store_dram(addr, size, val);
    else if(VIRTIO_BASE <= addr && addr < VIRTIO_BASE+VIRTIO_SIZE)
        return store_virtio(addr, size, val);
    else
        return (Result){.exception = StoreAMOAccessFault, .value = 0};
}