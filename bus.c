#include "bus.h"

Result load(u64 addr, u64 size) {
    if(CLINT_BASE <= addr && addr < CLINT_BASE+CLINT_SIZE)
        return load_clint(addr, size);
    else if(PLIC_BASE <= addr && addr < PLIC_BASE+PLIC_SIZE)
        return load_plic(addr, size);
    else if(UART_BASE <= addr && addr < UART_BASE+UART_SIZE)
        return load_uart(addr, size);
    else if(DRAM_BASE <= addr && addr < DRAM_BASE+DRAM_SIZE)
        return load_dram(addr, size);
    else
        return (Result){.exception = LoadAccessFault, .value = 0};
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
    else
        return (Result){.exception = StoreAMOAccessFault, .value = 0};
}