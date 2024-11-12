#ifndef BUS_H
#define BUS_H

#include "utils.h"
#include "clint.h"
#include "dram.h"
#include "plic.h"
#include "uart.h"

Result load(u64 addr, u64 size);

Result store(u64 addr, u64 size, u64 val);

#endif