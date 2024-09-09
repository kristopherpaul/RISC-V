#include "cpu.h"

CPU cpu;

void initCPU(){
    cpu.reg[2] = DRAM_SIZE;
    cpu.pc = 0;
}

inst decode(u32 ins){
    inst cur_inst;
    cur_inst.opcode = ins & 0x7f;
    cur_inst.rd = (ins>>7) & 0x1f;
    cur_inst.rs1 = (ins>>15) & 0x1f;
    cur_inst.rs2 = (ins>>20) & 0x1f;
    cur_inst.im5 = (ins>>7) & 0x1f;
    cur_inst.im7 = (ins>>25) & 0x7f;
    cur_inst.im12 = (ins>>20) & 0xfff;
    cur_inst.im20 = (ins>>12) & 0xfffff;
    return cur_inst;
}

u32 fetch(){
    u32 ins = cpu.dram[cpu.pc];
    if(ins == 0){
        fprintf(stderr, "ERROR: Instruction couldn't be fetched!");
        exit(-1);
    }
    #ifdef LITTLE_ENDIAN
        ins = ((ins&0xff) << 24) | (((ins>>8)&0xff) << 16) | (((ins>>16)&0xff) << 8) | ((ins>>24)&0xff);
    #endif
    return ins;
}

void execute(inst ins){
    switch(ins.opcode){
        case 0x33:
            // add
            cpu.reg[ins.rd] = cpu.reg[ins.rs1] + cpu.reg[ins.rs2];
            break;
        
        case 0x13:
            // addi
            cpu.reg[ins.rd] = cpu.reg[ins.rs1] + ins.im12;
            break;
        
        default:
            fprintf(stderr, "ERROR: Operation hasn't been implemented yet!");
            exit(-1);
            break;
    }
}