#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<pthread.h>
#include<stdatomic.h>
#include<unistd.h>

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

#define CLINT_BASE 0x2000000
#define CLINT_SIZE 0x10000

#define PLIC_BASE 0xc000000
#define PLIC_SIZE 0x4000000

#define CLINT_MTIMECMP CLINT_BASE+0x4000
#define CLINT_MTIME CLINT_BASE+0xbff8

#define PLIC_PENDING PLIC_BASE+0x1000
#define PLIC_SENABLE PLIC_BASE+0x2080
#define PLIC_SPRIORITY PLIC_BASE+0x201000
#define PLIC_SCLAIM PLIC_BASE+0x201004

#define UART_BASE 0x10000000
#define UART_SIZE 0x100

#define UART_RHR (UART_BASE + 0) // Receive holding register
#define UART_THR (UART_BASE + 0) // Transmit holding register
#define UART_LCR (UART_BASE + 3) // Line control register
#define UART_LSR (UART_BASE + 5) // Line status register

#define UART_LSR_RX (1 << 0) // Receiver ready
#define UART_LSR_TX (1 << 5) // Transmitter empty
#define UART_IRQ 10 // Interrupt Request of UART

#define VIRTIO_BASE 0x10001000 // The address which virtio starts.
#define VIRTIO_SIZE 0x1000 // The size of virtio.

#define VIRTIO_IRQ 1 // The interrupt request of virtio.
#define VRING_DESC_SIZE 16
#define DESC_NUM 8 // The number of virtio descriptors. It must be a power of two.
#define VIRTIO_MAGIC VIRTIO_BASE + 0x000 // Always return 0x74726976.
#define VIRTIO_VERSION VIRTIO_BASE + 0x004 // The version. 1 is legacy.
#define VIRTIO_DEVICE_ID VIRTIO_BASE + 0x008 // device type 1 is net, 2 is disk.
#define VIRTIO_VENDOR_ID VIRTIO_BASE + 0x00c // Always return 0x554d4551
#define VIRTIO_DEVICE_FEATURES VIRTIO_BASE + 0x010 // Device features.
#define VIRTIO_DRIVER_FEATURES VIRTIO_BASE + 0x020 // Driver features.
#define VIRTIO_GUEST_PAGE_SIZE VIRTIO_BASE + 0x028 // Page size for PFN, write-only.
#define VIRTIO_QUEUE_SEL VIRTIO_BASE + 0x030 // Select queue, write-only.
#define VIRTIO_QUEUE_NUM_MAX VIRTIO_BASE + 0x034 // Max size of current queue, read-only. In QEMU, `VIRTIO_COUNT = 8`.
#define VIRTIO_QUEUE_NUM VIRTIO_BASE + 0x038 // Size of current queue, write-only.
#define VIRTIO_QUEUE_PFN VIRTIO_BASE + 0x040 // Physical page number for queue, read and write.
#define VIRTIO_QUEUE_NOTIFY VIRTIO_BASE + 0x050 // Notify the queue number, write-only.
// Device status, read and write. Reading from this register returns the current device status flags.
// Writing non-zero values to this register sets the status flags, indicating the OS/driver
// progress. Writing zero (0x0) to this register triggers a device reset.
#define VIRTIO_STATUS VIRTIO_BASE + 0x070

#ifndef UTILS_H
#define UTILS_H

#define MIP_SSIP (1 << 1)
#define MIP_MSIP (1 << 3)
#define MIP_STIP (1 << 5)
#define MIP_MTIP (1 << 7)
#define MIP_SEIP (1 << 9)
#define MIP_MEIP (1 << 11)

#define VIRTIO_BASE 0x10001000
#define VIRTIO_SIZE 0x1000

#define VIRTIO_IRQ 1
#define VRING_DESC_SIZE 16
#define DESC_NUM 8

#define VIRTIO_MAGIC VIRTIO_BASE + 0x000
#define VIRTIO_VERSION VIRTIO_BASE + 0x004
#define VIRTIO_DEVICE_ID VIRTIO_BASE + 0x008
#define VIRTIO_VENDOR_ID VIRTIO_BASE + 0x00c
#define VIRTIO_DEVICE_FEATURES VIRTIO_BASE + 0x010
#define VIRTIO_DRIVER_FEATURES VIRTIO_BASE + 0x020
#define VIRTIO_GUEST_PAGE_SIZE VIRTIO_BASE + 0x028
#define VIRTIO_QUEUE_SEL VIRTIO_BASE + 0x030
#define VIRTIO_QUEUE_NUM_MAX VIRTIO_BASE + 0x034
#define VIRTIO_QUEUE_NUM VIRTIO_BASE + 0x038
#define VIRTIO_QUEUE_PFN VIRTIO_BASE + 0x040
#define VIRTIO_QUEUE_NOTIFY VIRTIO_BASE + 0x050
#define VIRTIO_STATUS VIRTIO_BASE + 0x070

#define PAGE_SIZE 4096

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
    NullException
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

typedef enum Interrupt{
    UserSoftwareInterrupt,
    SupervisorSoftwareInterrupt,
    MachineSoftwareInterrupt,
    UserTimerInterrupt,
    SupervisorTimerInterrupt,
    MachineTimerInterrupt,
    UserExternalInterrupt,
    SupervisorExternalInterrupt,
    MachineExternalInterrupt,
    NullInterrupt
} Interrupt;

static const char *Interrupts[] = {"UserSoftwareInterrupt",
                                   "SupervisorSoftwareInterrupt",
                                   "MachineSoftwareInterrupt",
                                   "UserTimerInterrupt",
                                   "SupervisorTimerInterrupt",
                                   "MachineTimerInterrupt",
                                   "UserExternalInterrupt",
                                   "SupervisorExternalInterrupt",
                                   "MachineExternalInterrupt"};

typedef struct RESULT {
    u64 value;
    Exception exception;
    Interrupt interrupt;
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