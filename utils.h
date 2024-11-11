#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

#pragma GCC diagnostic warning "-Wunused"

#define u64 uint64_t
#define i64 int64_t
#define i32 int32_t
#define u32 uint32_t
#define u16 uint16_t
#define i16 int16_t
#define u8 uint8_t
#define i8 int8_t

#define DRAM_SIZE 1024 * 1024 * 128
#define DRAM_BASE 0x80000000
#define UART_BASE 0x10000000
#define UART_SIZE 0x100

// Machine-level CSRs
#define MHARTID 0xf14 // Hardware thread ID
#define MSTATUS 0x300 // Machine status register
#define MEDELEG 0x302 // Machine exception delegation register
#define MIDELEG 0x303 // Machine interrupt delegation register
#define MIE 0x304 // Machine interrupt-enable register
#define MTVEC 0x305 // Machine trap-handler base address
#define MCOUNTEREN 0x306 // Machine counter enable
#define MSCRATCH 0x340 // Scratch register for machine trap handlers
#define MEPC 0x341 // Machine exception program counter
#define MCAUSE 0x342 // Machine trap cause
#define MTVAL 0x343 // Machine bad address or instruction
#define MIP 0x344 // Machine interrupt pending

// Supervisor-level CSRs
#define SSTATUS 0x100 // Supervisor status register
#define SIE 0x104 // Supervisor interrupt-enable register
#define STVEC 0x105 // Supervisor trap handler base address
#define SSCRATCH 0x140 // Scratch register for supervisor trap handlers
#define SEPC 0x141 // Supervisor exception program counter
#define SCAUSE 0x142 // Supervisor trap cause
#define STVAL 0x143 // Supervisor bad address or instruction
#define SIP 0x144 // Supervisor interrupt pending
#define SATP 0x180 // Supervisor address translation and protection

#define CLINT_BASE 0x20000000
#define CLINT_SIZE 0x10000

#define PLIC_BASE 0xc0000000
#define PLIC_SIZE 0x4000000

#define CLINT_MTIMECMP CLINT_BASE+0x4000
#define CLINT_MTIME CLINT_BASE+0xbff8

#define PLIC_PENDING PLIC_BASE+0x1000
#define PLIC_SENABLE PLIC_BASE+0x2080
#define PLIC_SPRIORITY PLIC_BASE+0x201000
#define PLIC_SCLAIM PLIC_BASE+0x201004

#ifndef UTILS_H
#define UTILS_H

typedef enum Exception{
    InstructionAddressMisaligned,
    InstructionAccessFault,
    IllegalInstruction,
    Breakpoint,
    LoadAddressMisaligned,
    LoadAccessFault,
    StoreAMOAddressMisaligned,
    StoreAMOAccessFault,
    EnvironmentCallFromUMode,
    EnvironmentCallFromSMode,
    EnvironmentCallFromMMode,
    InstructionPageFault,
    LoadPageFault,
    StoreAMOPageFault,
    Null
} Exception;

static const char *Exceptions[] = {"InstructionAddressMisaligned",
                                "InstructionAccessFault", 
                                "IllegalInstruction", 
                                "Breakpoint", 
                                "LoadAddressMisaligned",
                                "LoadAccessFault",
                                "StoreAMOAddressMisaligned",
                                "StoreAMOAccessFault",
                                "EnvironmentCallFromUMode",
                                "EnvironmentCallFromSMode",
                                "EnvironmentCallFromMMode",
                                "InstructionPageFault",
                                "LoadPageFault",
                                "StoreAMOPageFault"};

typedef struct RESULT {
    u64 value;
    Exception exception;
} Result;

static i32 sext32(u32 rest_num, u32 sign, u8 num_bits){
    u32 val = 0;
    for(u8 i = 0;i < num_bits;i++){
        val <<= 1;
        val |= sign&1;
    }
    val <<= (32-num_bits);
    val |= rest_num;
    return (i32)val;
}

static i64 sext64(u64 rest_num, u64 sign, u8 num_bits){
    u64 val = 0;
    for(u8 i = 0;i < num_bits;i++){
        val <<= 1;
        val |= sign&1;
    }
    val <<= (64-num_bits);
    val |= rest_num;
    return (i64)val;
}

#endif