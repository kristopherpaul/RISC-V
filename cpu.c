#include "cpu.h"
#include "dram.h"

CPU cpu;

void initCPU(){
    cpu.reg[2] = DRAM_BASE + DRAM_SIZE;
    //cpu.pc = 0;
    cpu.pc = DRAM_BASE;
    cpu.mode = Machine;
}

i32 sext32(u32 rest_num, u32 sign, u8 num_bits){
    u32 val = 0;
    for(u8 i = 0;i < num_bits;i++){
        val <<= 1;
        val |= sign&1;
    }
    val <<= (32-num_bits);
    val |= rest_num;
    return (i32)val;
}

inst decode(u32 ins){
    inst cur_inst;
    cur_inst.opcode = ins & 0x7f;
    cur_inst.rd = (ins>>7) & 0x1f;
    cur_inst.funct3 = (ins>>12) & 0x7;
    cur_inst.rs1 = (ins>>15) & 0x1f;
    cur_inst.rs2 = (ins>>20) & 0x1f;
    cur_inst.shamt5 = cur_inst.rs2;
    cur_inst.shamt6 = (ins>>20) & 0x3f;
    cur_inst.funct7 = (ins>>25) & 0x7f;
    cur_inst.funct5 = (ins>>27) & 0x1f;
    cur_inst.aq = cur_inst.funct7 & 0x2;
    cur_inst.rl = cur_inst.funct7 & 0x1;
    
    cur_inst.imI = sext32(((ins>>20) & 0x7ff), ins>>31, 21);
    cur_inst.imS = sext32((((ins>>25) & 0x3f) << 5) | (((ins>>8) & 0xf) << 1) | ((ins>>7) & 0x1), ins>>31, 21);
    cur_inst.imB = sext32((((ins>>25) & 0x3f) << 5) | (((ins>>8) & 0xf) << 1) | (((ins>>7) & 0x1) << 11), ins>>31, 20);
    cur_inst.imU = sext32((((ins>>20) & 0x7ff) << 20) | (((ins>>12) & 0xff) << 12), ins>>31, 1);
    cur_inst.imJ = sext32((((ins>>21) & 0xf) << 1) | (((ins>>25) & 0x3f) << 5) | (((ins>>20) & 0x1) << 11) | (((ins>>12) & 0xff) << 12), ins>>31, 12);

    cur_inst.csraddr = (ins & 0xfff00000) >> 20;
    return cur_inst;
}

u32 fetch(){
    u32 ins = 0;
    if(cpu.pc < DRAM_BASE){
        return ins;
    }
    for(int i = 0;i < 4;i++){
        ins |= dram.mem[cpu.pc-DRAM_BASE+i] << (i*8);
    }
    /*if(ins == 0){
        fprintf(stderr, "ERROR: Instruction couldn't be fetched!");
        exit(-1);
    }*/
    #ifdef LITTLE_ENDIAN
        ins = ((ins&0xff) << 24) | (((ins>>8)&0xff) << 16) | (((ins>>16)&0xff) << 8) | ((ins>>24)&0xff);
    #endif
    return ins;
}

u64 load_csr(u64 addr){
    switch(addr){
        case SIE:
            return cpu.csr[MIE] & cpu.csr[MIDELEG]; 
        
        default:
            return cpu.csr[addr];
    }
}

void store_csr(u64 addr, u64 val){
    switch(addr){
        case SIE:
            cpu.csr[MIE] = (cpu.csr[MIE] & !cpu.csr[MIDELEG]) | (val & cpu.csr[MIDELEG]); 
            break;
        
        default:
            cpu.csr[addr] = val;
            break;
    }
}

void dump_regs(){
    char* reg_names[] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",
                        "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5",
                        "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
    for(int i = 0;i < 32;i++){
        fprintf(stderr,"x%d (%s) = %llu, %lld\n",i,reg_names[i],cpu.reg[i],cpu.reg[i]);
    }
}

void dump_csrs(){
    fprintf(stderr,"MSTATUS = %llu\nMTVEC = %llu\nMEPC = %llu\nMCAUSE = %llu\n",load_csr(MSTATUS),load_csr(MTVEC),load_csr(MEPC),load_csr(MCAUSE));
    fprintf(stderr,"SSTATUS = %llu\nSTVEC = %llu\nSEPC = %llu\nSCAUSE = %llu\n",load_csr(SSTATUS),load_csr(STVEC),load_csr(SEPC),load_csr(SCAUSE));
}

void execute(inst ins){
    cpu.reg[0] = 0;
    u64 addr;
    u64 val;
    u32 shamt = (u32)(u64)(cpu.reg[ins.rs2] & 0x3f);
    switch(ins.opcode){
        case 0x33: //R-type
            switch(ins.funct3){
                case 0x0:
                    switch(ins.funct7){
                        case 0x00: //add
                            cpu.reg[ins.rd] = cpu.reg[ins.rs1] + cpu.reg[ins.rs2];
                            break;
                        case 0x01: //mul
                            cpu.reg[ins.rd] = cpu.reg[ins.rs1] * cpu.reg[ins.rs2];
                            break;
                        case 0x20: //sub
                            cpu.reg[ins.rd] = cpu.reg[ins.rs1] - cpu.reg[ins.rs2];
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                case 0x1: //sll
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] << shamt;
                    break;
                case 0x2: //slt
                    cpu.reg[ins.rd] = (i64)cpu.reg[ins.rs1] < (i64)cpu.reg[ins.rs2] ? 1 : 0;
                    break;
                case 0x3: //sltu
                    cpu.reg[ins.rd] = (u64)cpu.reg[ins.rs1] < (u64)cpu.reg[ins.rs2] ? 1 : 0;
                    break;
                case 0x4: //xor
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] ^ cpu.reg[ins.rs2];
                    break;
                case 0x5:
                    switch(ins.funct7){
                        case 0x00: //srl
                            cpu.reg[ins.rd] = cpu.reg[ins.rs1] >> shamt;
                            break;
                        case 0x20: //sra
                            cpu.reg[ins.rd] = (u64)((i64)cpu.reg[ins.rs1] >> shamt);
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                case 0x6: //or
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] | cpu.reg[ins.rs2];
                    break;
                case 0x7: //and
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] & cpu.reg[ins.rs2];
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;
        
        case 0x23: //S-type
            addr = cpu.reg[ins.rs1] + ins.imS;
            switch(ins.funct3){
                case 0x0: //sb
                    store(addr, 8, cpu.reg[ins.rs2]);
                    break;
                case 0x1: //sh
                    store(addr, 16, cpu.reg[ins.rs2]);
                    break;
                case 0x2: //sw
                    store(addr, 32, cpu.reg[ins.rs2]);
                    break;
                case 0x3: //sd
                    store(addr, 64, cpu.reg[ins.rs2]);
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;
        
        case 0x13: //I-type
            switch(ins.funct3){
                case 0x0: //addi
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] + ins.imI;
                    break;
                case 0x1: //slli
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] << ins.shamt6;
                    break;
                case 0x2: //slti
                    cpu.reg[ins.rd] = (i64)cpu.reg[ins.rs1] < (i64)ins.imI ? 1 : 0;
                    break;
                case 0x3: //sltiu
                    cpu.reg[ins.rd] = (u64)cpu.reg[ins.rs1] < (u64)ins.imI ? 1 : 0;
                    break;
                case 0x4: //xori
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] ^ ins.imI;
                    break;
                case 0x5:
                    switch(ins.funct7>>1){
                        case 0x00: //srli
                            cpu.reg[ins.rd] = (u64)cpu.reg[ins.rs1] >> ins.shamt6;
                            break;
                        case 0x10: //srai
                            cpu.reg[ins.rd] = (u64)((i64)cpu.reg[ins.rs1] >> ins.shamt6);
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                case 0x6: //ori
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] | ins.imI;
                    break;
                case 0x7: //andi
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] & ins.imI;
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;
        
        case 0x03: //Load
            addr = cpu.reg[ins.rs1] + ins.imI;
            switch(ins.funct3){
                case 0x0: //lb
                    cpu.reg[ins.rd] = (u64)(i64)(i8)load(addr, 8);
                    break;
                case 0x1: //lh
                    cpu.reg[ins.rd] = (u64)(i64)(i16)load(addr, 16);
                    break;
                case 0x2: //lw
                    cpu.reg[ins.rd] = (u64)(i64)(i32)load(addr, 32);
                    break;
                case 0x3: //ld
                    cpu.reg[ins.rd] = load(addr, 64);
                    break;
                case 0x4: //lbu
                    cpu.reg[ins.rd] = load(addr, 8);
                    break;
                case 0x5: //lhu
                    cpu.reg[ins.rd] = load(addr, 16);
                    break;
                case 0x6: //lwu
                    cpu.reg[ins.rd] = load(addr, 32);
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;
        
        case 0x17: //auipc
            cpu.reg[ins.rd] = cpu.pc + ins.imU - 4;
            break;
        
        case 0x1b:
            switch(ins.funct3){
                case 0x0: //addiw
                    cpu.reg[ins.rd] = (u64)(i64)(i32)(cpu.reg[ins.rs1] + (u64)(i64)(i32)(ins.imI));
                    break;
                case 0x1: //slliw
                    cpu.reg[ins.rd] = (u64)(i64)(i32)(cpu.reg[ins.rs1] << ins.shamt5);
                    break;
                case 0x5:
                    switch(ins.funct7){
                        case 0x00: //srliw
                            cpu.reg[ins.rd] = (u64)(i64)(i32)((u32)cpu.reg[ins.rs1] >> ins.shamt5);
                            break;
                        case 0x20: //sraiw
                            cpu.reg[ins.rd] = (u64)(i64)((i32)cpu.reg[ins.rs1] >> ins.shamt5);
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;

        case 0x37: //lui
            cpu.reg[ins.rd] = (u64)(i64)(i32)ins.imU;
            break;
        
        case 0x3b:
            switch(ins.funct3){
                case 0x0:
                    switch(ins.funct7){
                        case 0x00: //addw
                            cpu.reg[ins.rd] = (u64)(i64)(i32)(cpu.reg[ins.rs1] + cpu.reg[ins.rs2]);
                            break;
                        case 0x20: //subw
                            cpu.reg[ins.rd] = (u64)(i32)(cpu.reg[ins.rs1] - cpu.reg[ins.rs2]);
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                case 0x1: //sllw
                    cpu.reg[ins.rd] = (u64)(i32)((u32)(cpu.reg[ins.rs1]) << cpu.reg[ins.rs2]);
                    break;
                case 0x5:
                    switch(ins.funct7){
                        case 0x00: //srlw
                            cpu.reg[ins.rd] = (u64)(i32)((u32)(cpu.reg[ins.rs1]) >> cpu.reg[ins.rs2]);
                            break;
                        case 0x01: //divu
                            if(cpu.reg[ins.rs2] == 0){
                                //divison by 0, set DZ csr flag
                                cpu.reg[ins.rd] = 0xffffffffffffffff;
                            }else{
                                cpu.reg[ins.rd] = cpu.reg[ins.rs1]/cpu.reg[ins.rs2];
                            }
                            break;
                        case 0x20: //sraw
                            cpu.reg[ins.rd] = (u64)((i32)cpu.reg[ins.rs1] >> (i32)cpu.reg[ins.rs2]);
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                case 0x7:
                    switch(ins.funct7){
                        case 0x01: //remuw
                            if(cpu.reg[ins.rs2] == 0){
                                //divison by 0, set DZ csr flag
                                cpu.reg[ins.rd] = cpu.reg[ins.rs1];
                            }else{
                                cpu.reg[ins.rd] = (u64)(i32)((u32)(cpu.reg[ins.rs1]) % (u32)(cpu.reg[ins.rs2]));
                            }
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;

        case 0x63:
            switch(ins.funct3){
                case 0x0: //beq
                    if(cpu.reg[ins.rs1] == cpu.reg[ins.rs2]){
                        cpu.pc += ins.imB - 4;
                    }
                    break;
                case 0x1: //bne
                    if(cpu.reg[ins.rs1] != cpu.reg[ins.rs2]){
                        cpu.pc += ins.imB - 4;
                    }
                    break;
                case 0x4: //blt
                    if((i64)cpu.reg[ins.rs1] < (i64)cpu.reg[ins.rs2]){
                        cpu.pc += ins.imB - 4;
                    }
                    break;
                case 0x5: //bge
                    if((i64)cpu.reg[ins.rs1] >= (i64)cpu.reg[ins.rs2]){
                        cpu.pc += ins.imB - 4;
                    }
                    break;
                case 0x6: //bltu
                    if(cpu.reg[ins.rs1] < cpu.reg[ins.rs2]){
                        cpu.pc += ins.imB - 4;
                    }
                    break;
                case 0x7: //bgeu
                    if(cpu.reg[ins.rs1] >= cpu.reg[ins.rs2]){
                        cpu.pc += ins.imB - 4;
                    }
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;
        
        case 0x67: //jalr
            addr = (cpu.reg[ins.rs1] + ins.imI) & ~1;
            cpu.reg[ins.rd] = cpu.pc;
            cpu.pc = addr;
            break;

        case 0x6f: //jal
            cpu.reg[ins.rd] = cpu.pc;
            cpu.pc += ins.imJ - 4;
            break;

        case 0x73:
            switch(ins.funct3){
                case 0x0:
                    switch(ins.rs2){
                        case 0x2:
                            switch(ins.funct7){
                                case 0x8: //sret
                                    cpu.pc = load_csr(SEPC);
                                    if(((load_csr(SSTATUS)>>8) & 0x1) == 1){
                                        cpu.mode = Supervisor;
                                    }else{
                                        cpu.mode = User;
                                    }
                                    if(((load_csr(SSTATUS)>>5) & 0x1) == 1){
                                        store_csr(SSTATUS, load_csr(SSTATUS) | (1<<1));
                                    }else{
                                        store_csr(SSTATUS, load_csr(SSTATUS) & !(1<<1));
                                    }
                                    store_csr(SSTATUS, load_csr(SSTATUS) | (1<<5));
                                    store_csr(SSTATUS, load_csr(SSTATUS) & !(1<<8));
                                    break;
                                case 0x18: //mret
                                    cpu.pc = load_csr(MEPC);
                                    switch((load_csr(MSTATUS)>>11) & 0x3){
                                        case 1:
                                            cpu.mode = Supervisor;
                                            break;
                                        case 2:
                                            cpu.mode = Machine;
                                            break;
                                        default:
                                            cpu.mode = User;
                                            break;
                                    }
                                    if(((load_csr(MSTATUS)>>7) & 1) == 1){
                                        store_csr(MSTATUS, load_csr(MSTATUS) | (1<<3));
                                    }else{
                                        store_csr(MSTATUS, load_csr(MSTATUS) & !(1<<3));
                                    }
                                    store_csr(MSTATUS, load_csr(MSTATUS) | (1<<7));
                                    store_csr(MSTATUS, load_csr(MSTATUS) & !(0x3<<11));
                                    break;
                                default:
                                    dump_regs();
                                    dump_csrs();
                                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                                    exit(-1);
                                    break;
                            }
                            break;
                        default:
                            switch(ins.funct7){
                                case 0x9:
                                    //sfence.vma
                                    break;
                                default:
                                    dump_regs();
                                    dump_csrs();
                                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                                    exit(-1);
                                    break;
                            }
                            break;
                    }
                    break;
                case 0x1: //csrrw
                    val = load_csr(ins.csraddr);
                    store_csr(ins.csraddr, cpu.reg[ins.rs1]);
                    cpu.reg[ins.rd] = val;
                    break;
                case 0x2: //csrrs
                    val = load_csr(ins.csraddr);
                    store_csr(ins.csraddr, val | cpu.reg[ins.rs1]);
                    cpu.reg[ins.rd] = val;
                    break;
                case 0x3: //csrrc
                    val = load_csr(ins.csraddr);
                    store_csr(ins.csraddr, val & (!cpu.reg[ins.rs1]));
                    cpu.reg[ins.rd] = val;
                    break;
                case 0x5: //csrrwi
                    cpu.reg[ins.rd] = load_csr(ins.csraddr);
                    store_csr(ins.csraddr, (u64)ins.rs1);
                    break;
                case 0x6: //csrrsi
                    val = load_csr(ins.csraddr);
                    store_csr(ins.csraddr, val | ((u64)ins.rs1));
                    cpu.reg[ins.rd] = val;
                    break;
                case 0x7: //csrrci
                    val = load_csr(ins.csraddr);
                    store_csr(ins.csraddr, val & (!((u64)ins.rs1)));
                    cpu.reg[ins.rd] = val;
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;

        case 0x2f: //Atomic instructions
            switch(ins.funct5){
                case 0x00:
                    switch(ins.funct3){
                        case 0x2: //amoadd.w
                            val = load(cpu.reg[ins.rs1], 32) + cpu.reg[ins.rs2];
                            store(cpu.reg[ins.rs1], 32, val);
                            cpu.reg[ins.rd] = val;
                            break;
                        case 0x3: //amoadd.d
                            val = load(cpu.reg[ins.rs1], 64) + cpu.reg[ins.rs2];
                            store(cpu.reg[ins.rs1], 64, val);
                            cpu.reg[ins.rd] = val;
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                case 0x01:
                    switch(ins.funct3){
                        case 0x2: //amoswap.w
                            val = load(cpu.reg[ins.rs1], 32);
                            store(cpu.reg[ins.rs1], 32, cpu.reg[ins.rs2]);
                            cpu.reg[ins.rd] = val;
                            break;
                        case 0x3: //amoswap.d
                            val = load(cpu.reg[ins.rs1], 64);
                            store(cpu.reg[ins.rs1], 64, cpu.reg[ins.rs2]);
                            cpu.reg[ins.rd] = val;
                            break;
                        default:
                            dump_regs();
                            dump_csrs();
                            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                            exit(-1);
                            break;
                    }
                    break;
                default:
                    dump_regs();
                    dump_csrs();
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
                    exit(-1);
                    break;
            }
            break;

        case 0x0:
            break;

        default:
            dump_regs();
            dump_csrs();
            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
            exit(-1);
            break;
    }
}