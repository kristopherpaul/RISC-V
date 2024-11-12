#! /bin/bash
gcc main.c dram.c cpu.c trap.c clint.c plic.c -o main
gcc test.c -o test
test
test rv64ui
test rv64ua
test rv64um