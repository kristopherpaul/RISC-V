#ifndef CPU_H
#define CPU_H

#include "utils.h"

typedef struct INST {
    u8 opcode;
    u8 rd, rs1, rs2;
    i32 im5, im7, im12, im20;
} inst;

typedef struct CPU {
    u64 reg[32];        
    u32 pc;             
    u32 dram[1024*1024];
} CPU;

extern CPU cpu;  

void initCPU();

inst decode(u32 ins);

u32 fetch();

void execute(inst ins);
#endif