#include "trap.h"

u64 trap_code(void *e, bool interrupt){
    if(!interrupt){
        switch(*((Exception*)e)){
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
            default:
                return -1;
        }
    }else{
        switch(*((Interrupt*)e)){
            case UserSoftwareInterrupt:
                return 0;
            case SupervisorSoftwareInterrupt:
                return 1;
            case MachineSoftwareInterrupt:
                return 3;
            case UserTimerInterrupt:
                return 4;
            case SupervisorTimerInterrupt:
                return 5;
            case MachineTimerInterrupt:
                return 7;
            case UserExternalInterrupt:
                return 8;
            case SupervisorExternalInterrupt:
                return 9;
            case MachineExternalInterrupt:
                return 11;
            default:
                return -1;
        }
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

void take_trap(void *e, bool interrupt){
    u32 trap_pc = cpu.pc-4;
    Mode prev_mode = cpu.mode;
    u32 code = trap_code(e, interrupt);
    if(interrupt){
        code = ((u64)1<<63)|code;
    }
    if((prev_mode <= Supervisor) && (((load_csr(MEDELEG) >> code)&1) != 0)){
        cpu.mode = Supervisor;
        if(interrupt){
            u64 val;
            switch(load_csr(STVEC)&1){
                case 1:
                    val = 4*code;
                    break;
                default:
                    val = 0;
            }
            cpu.pc = (load_csr(STVEC) & !1) + val;
        }else{
            cpu.pc = load_csr(STVEC) & !1;   
        }
        store_csr(SEPC, trap_pc & !1);
        store_csr(SCAUSE, code);
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
        if(interrupt){
            u64 val;
            switch(load_csr(MTVEC)&1){
                case 1:
                    val = 4*code;
                    break;
                default:
                    val = 0;
            }
            cpu.pc = (load_csr(MTVEC) & !1) + val;
        }else{
            cpu.pc = load_csr(MTVEC) & !1;   
        }
        store_csr(MEPC, trap_pc & !1);
        store_csr(MCAUSE, code);
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

void take_exception(Exception e){
    take_trap(&e, false);
}

void take_interrupt(Interrupt i){
    take_trap(&i, true);
}