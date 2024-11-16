#! /bin/bash
cd src & gcc main.c dram.c cpu.c trap.c plic.c clint.c uart.c bus.c virtio.c -o ../main