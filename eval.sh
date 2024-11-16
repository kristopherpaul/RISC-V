#! /bin/bash
cd src & gcc main.c dram.c cpu.c trap.c clint.c plic.c uart.c bus.c -o main
cd .. & gcc test.c -o test
test
test rv64u