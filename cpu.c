#include "cpu.h"
#include "dram.h"

CPU cpu;

void initCPU(){
    cpu.reg[2] = DRAM_BASE + DRAM_SIZE;
    //cpu.pc = 0;
    cpu.pc = DRAM_BASE;
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
    
    cur_inst.imI = sext32(((ins>>20) & 0x7ff), ins>>31, 21);
    cur_inst.imS = sext32((((ins>>25) & 0x3f) << 5) | (((ins>>8) & 0xf) << 1) | ((ins>>7) & 0x1), ins>>31, 21);
    cur_inst.imB = sext32((((ins>>25) & 0x3f) << 5) | (((ins>>8) & 0xf) << 1) | (((ins>>7) & 0x1) << 11), ins>>31, 20);
    cur_inst.imU = sext32((((ins>>20) & 0x7ff) << 20) | (((ins>>12) & 0xff) << 12), ins>>31, 1);
    cur_inst.imJ = sext32((((ins>>21) & 0xf) << 1) | (((ins>>25) & 0x3f) << 5) | (((ins>>20) & 0x1) << 11) | (((ins>>12) & 0xff) << 12), ins>>31, 12);

    return cur_inst;
}

u32 fetch(){
    u32 ins = 0;
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

void dump_regs(){
    char* reg_names[] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",
                        "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5",
                        "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
    for(int i = 0;i < 32;i++){
        fprintf(stderr,"x%d (%s) = %llu, %lld\n",i,reg_names[i],cpu.reg[i],cpu.reg[i]);
    }
}

void execute(inst ins){
    cpu.reg[0] = 0;
    u64 addr;
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
                    }
                    break;
                case 0x1: //sll
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] << ins.shamt5;
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
                            cpu.reg[ins.rd] = cpu.reg[ins.rs1] >> ins.shamt5;
                            break;
                        case 0x20: //sra
                            cpu.reg[ins.rd] = (u64)((i64)cpu.reg[ins.rs1] >> ins.shamt5);
                            break;
                    }
                    break;
                case 0x6: //or
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] | cpu.reg[ins.rs2];
                    break;
                case 0x7: //and
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] & cpu.reg[ins.rs2];
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
                    }
                    break;
                case 0x6: //ori
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] | ins.imI;
                    break;
                case 0x7: //andi
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] & ins.imI;
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
                    }
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
                    }
                    break;
                case 0x1: //sllw
                    cpu.reg[ins.rd] = (u64)(i32)((u32)(cpu.reg[ins.rs1]) << ins.shamt5);
                    break;
                case 0x5:
                    switch(ins.funct7){
                        case 0x00: //srlw
                            cpu.reg[ins.rd] = (u64)(i32)((u32)(cpu.reg[ins.rs1]) >> ins.shamt5);
                            break;
                        case 0x20: //sraw
                            cpu.reg[ins.rd] = (u64)((i32)cpu.reg[ins.rs1] >> (i32)ins.shamt5);
                            break;
                    }
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

        case 0x0:
            dump_regs();
            exit(0);
            break;

        default:
            dump_regs();
            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
            exit(-1);
            break;
    }
}