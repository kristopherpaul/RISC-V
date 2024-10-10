#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

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