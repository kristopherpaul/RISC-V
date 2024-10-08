#include "cpu.h"
#include "utils.h"
#include "dram.h"

//#define LITTLE_ENDIAN

// 32-bit

void loadROM(char* fileName){
    FILE *file_ptr = fopen(fileName, "rb");
    u8 buffer;
    while(fread(&buffer, sizeof(buffer), 1, file_ptr)){
        dram.mem[cpu.pc] = buffer;
        cpu.pc++;
    }
    fclose(file_ptr);
}

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Usage: main <filename>");
        return 0;
    }
    cpu.pc = 0;
    loadROM(argv[1]);
    initCPU();
    // fetch-decode-execute cycle
    while(cpu.pc < (sizeof(dram.mem)/sizeof(dram.mem[0]))){
        u32 cur_inst = fetch();
        cpu.pc += 4;
        inst parsed_inst = decode(cur_inst);
        // debug mode
        if(argc == 3 && argv[2][0] == '-' && argv[2][1] == 'd'){
            printf("-------------\n");
            printf("pc:%u\n",cpu.pc);
            printf("inst:%u\n",cur_inst);
            printf("opcode:%u | ",parsed_inst.opcode);
            printf("rd:%u | ",parsed_inst.rd);
            printf("rs1:%u | ",parsed_inst.rs1);
            printf("rs2:%u\n",parsed_inst.rs2);
            printf("im20:%d | ",parsed_inst.im20);
            printf("im20j:%d | ",parsed_inst.im20j);
            printf("im12:%d | ",parsed_inst.im12);
            printf("im12b:%d | ",parsed_inst.im12b);
            printf("im12s:%d\n",parsed_inst.im12s);
            printf("funct3:%u | ",parsed_inst.funct3);
            printf("funct7:%u | ",parsed_inst.funct7);
            printf("shamt5:%u | ",parsed_inst.shamt5);
            printf("shamt6:%u\n",parsed_inst.shamt6);
            dump_regs();
            char rep;
            scanf("%c",&rep);
        }
        execute(parsed_inst);
    }
    dump_regs();
    return 0;
}