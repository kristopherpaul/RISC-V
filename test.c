#include "stdbool.h"
#include "cpu.h"
#include "utils.h"

void loadROM(char* fileName){
    FILE *file_ptr = fopen(fileName, "rb");
    u32 buffer;
    while(fread(&buffer, sizeof(buffer), 1, file_ptr)){
        cpu.dram[cpu.pc] = buffer;
        cpu.pc++;
    }
    fclose(file_ptr);
}

int main(int argn, char* argc[]) {
    if(argn != 3){
        printf("Usage: ./test <binary_program_filename> <expected_register_filename>");
        return 0;
    }
    loadROM(argc[1]);
    initCPU();
    // fetch-decode-execute cycle
    while(cpu.pc < (sizeof(cpu.dram)/sizeof(cpu.dram[0]))){
        u32 cur_inst = fetch();
        if(cur_inst==0) {
            fprintf(stderr, "ERROR: Instruction couldn't be fetched!\n");
            break;
        }
        cpu.pc++;
        inst parsed_inst = decode(cur_inst);
        execute(parsed_inst);
    }
    FILE *out_ptr = fopen(argc[2], "r");
    char buffer[10];
    char* stop;
    u32 bufNum;
    bool match = true;
    for(int i = 0; i<32; i++){
        fgets(buffer, 10, out_ptr);
        bufNum = (u32) strtoull(buffer, &stop, 10);
        if(cpu.reg[i] != bufNum) {
            printf("Value at register %d is unexpected.\n", i);
            match = false;
        }
    }
    if(match)
        printf("Tests passed successfully.\n");
    else
        printf("Tests failed.\n");
    fclose(out_ptr);
    return 0;
}