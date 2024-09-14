cmake_minimum_required(VERSION 3.10)
project(RISC-V VERSION 0.01)

enable_language(C)
enable_language(CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(main "main.c" "cpu.c" "dram.c")