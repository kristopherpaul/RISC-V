#ifndef CPU_H
#define CPU_H

#include "utils.h"

typedef struct INST {
    u8 opcode;
    u8 rd, rs1, rs2;
    u8 funct3, funct7, shamt5, shamt6;
    i32 imI, imS, imB, imU, imJ;
    u64 csraddr;
} inst;

typedef struct CPU {
    u64 reg[32];
    u64 csr[4096];
    u32 pc;
} CPU;

extern CPU cpu;  

void initCPU();

inst decode(u32 ins);

u64 load_csr(u64 addr);

void store_csr(u64 addr, u64 val);

void dump_regs();

void dump_csrs();

u32 fetch();

void execute(inst ins);
#endif