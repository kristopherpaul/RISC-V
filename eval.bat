gcc main.c dram.c cpu.c trap.c clint.c plic.c uart.c bus.c -o main.exe
gcc test.c -o test.exe
test.exe
test.exe rv64ui
test.exe rv64ua
test.exe rv64um