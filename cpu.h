#ifndef CPU_H
#define CPU_H

#include "utils.h"

typedef struct INST {
    u8 opcode;
    u8 rd, rs1, rs2;
    u8 funct3, funct7, shamt5, shamt6;
    i32 imI, imS, imB, imU, imJ;
} inst;

typedef struct CPU {
    i64 reg[32];
    u32 pc;
} CPU;

extern CPU cpu;  

void initCPU();

inst decode(u32 ins);

void dump_regs();

u32 fetch();

void execute(inst ins);
#endif