#ifndef CPU_H
#define CPU_H

#include "utils.h"

typedef struct INST {
    u8 opcode;
    u8 rd, rs1, rs2;
    u8 funct3, funct5, funct7, shamt5, shamt6;
    i32 imI, imS, imB, imU, imJ;
    u64 csraddr;
    u8 aq, rl;
} inst;

typedef enum Mode{
    User = 0b00,
    Supervisor = 0b01,
    Machine = 0b11
} Mode;

typedef struct CPU {
    u64 reg[32];
    u64 csr[4096];
    u32 pc;
    Mode mode;
} CPU;

extern CPU cpu;  

void initCPU();

inst decode(u32 ins);

u64 load_csr(u64 addr);

void store_csr(u64 addr, u64 val);

void dump_regs();

void dump_csrs();

Result fetch();

Result execute(inst ins);
#endif