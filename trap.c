#include "trap.h"

u64 exception_code(Exception e){
    switch(e){
        case InstructionAddressMisaligned:
            return 0;
        case InstructionAccessFault:
            return 1;
        case IllegalInstruction:
            return 2;
        case Breakpoint:
            return 3;
        case LoadAddressMisaligned:
            return 4;
        case LoadAccessFault:
            return 5;
        case StoreAMOAddressMisaligned:
            return 6;
        case StoreAMOAccessFault:
            return 7;
        case EnvironmentCallFromUMode:
            return 8;
        case EnvironmentCallFromSMode:
            return 9;
        case EnvironmentCallFromMMode:
            return 11;
        case InstructionPageFault:
            return 12;
        case LoadPageFault:
            return 13;
        case StoreAMOPageFault:
            return 15;
        case Null:
            return -1;
    }
}

u8 is_fatal(Exception e){
    Exception fatal[5] = {InstructionAddressMisaligned, InstructionAccessFault, LoadAccessFault, StoreAMOAddressMisaligned, StoreAMOAccessFault};
    for(int i = 0;i < 5;i++){
        if(e == fatal[i]){
            return 1;
        }
    }
    return 0;
}

void take_trap(Exception e){
    u32 except_pc = cpu.pc-4;
    Mode prev_mode = cpu.mode;
    u32 except_code = exception_code(e);
    if((prev_mode <= Supervisor) && (((load_csr(MEDELEG) >> except_code)&1) != 0)){
        cpu.mode = Supervisor;
        cpu.pc = load_csr(STVEC) & !1;
        store_csr(SEPC, except_pc & !1);
        store_csr(SCAUSE, except_code);
        store_csr(STVAL, 0);
        u64 val;
        if(((load_csr(SSTATUS)>>1) & 1) == 1){
            val = load_csr(SSTATUS) | (1<<5);
        }else{
            val = load_csr(SSTATUS) | !(1<<5);
        }
        store_csr(SSTATUS, val);
        store_csr(SSTATUS, load_csr(SSTATUS) & !(1<<1));
        switch(prev_mode){
            case User:
                store_csr(SSTATUS, load_csr(SSTATUS) & !(1<<8));
                break;
            default:
                store_csr(SSTATUS, load_csr(SSTATUS) | (1<<8));
        }
    }else{
        cpu.mode = Machine;
        cpu.pc = load_csr(MTVEC) & !1;
        store_csr(MEPC, except_pc & !1);
        store_csr(MCAUSE, except_code);
        store_csr(MTVAL, 0);
        u64 val;
        if(((load_csr(MSTATUS)>>3) & 1) == 1){
            val = load_csr(MSTATUS) | (1<<7);
        }else{
            val = load_csr(MSTATUS) | !(1<<7);
        }
        store_csr(MSTATUS, val);
        store_csr(MSTATUS, load_csr(MSTATUS) & !(1<<3));
        store_csr(MSTATUS, load_csr(MSTATUS) & !(0b11<<11));
    }
}