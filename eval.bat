cd src & gcc main.c dram.c cpu.c trap.c clint.c plic.c uart.c bus.c virtio.c -o main.exe
cd.. & gcc test.c -o test.exe
test.exe
test.exe rv64u