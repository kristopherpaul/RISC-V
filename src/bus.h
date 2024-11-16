#ifndef BUS_H
#define BUS_H

#include "utils.h"
#include "cpu.h"
#include "clint.h"
#include "dram.h"
#include "plic.h"
#include "uart.h"
#include "virtio.h"

Result tload(u64 addr, u64 size, Access access);

Result tstore(u64 addr, u64 size, u64 val, Access access);

Result load(u64 addr, u64 size);

Result store(u64 addr, u64 size, u64 val);

#endif