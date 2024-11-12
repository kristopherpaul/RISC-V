#ifndef TRAP_H
#define TRAP_H

#include "utils.h"
#include "cpu.h"

u64 exception_code(Exception e);

u8 is_fatal(Exception e);

void take_exception(Exception e);

void take_interrupt(Interrupt i);

#endif