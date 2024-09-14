#include "cpu.h"
#include "utils.h"

//#define LITTLE_ENDIAN

// 32-bit

void loadROM(char* fileName){
    FILE *file_ptr = fopen(fileName, "rb");
    u32 buffer;
    while(fread(&buffer, sizeof(buffer), 1, file_ptr)){
        cpu.dram[cpu.pc] = buffer;
        cpu.pc++;
    }
    fclose(file_ptr);
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage: ./main <filename>");
        return 0;
    }
    loadROM(argv[1]);
    /*printf("Instructions:\n");
    for(int i = 0;i < cpu.pc;i++){
        printf("%d\n",cpu.dram[i]);
    }*/
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
    printf("Register contents at the end of execution:\n");
    for(int i = 0; i<32; i++)
        printf("%llu\n", cpu.reg[i]);
    printf("\n");
    return 0;
}