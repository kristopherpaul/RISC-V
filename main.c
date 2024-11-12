#include "cpu.h"
#include "utils.h"
#include "dram.h"
#include "trap.h"

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
    while(cpu.pc != 0){
        cpu.reg[0] = 0;
        Result cur_inst_res = fetch();
        u32 cur_inst;
        if(cur_inst_res.exception != NullException){
            take_trap(cur_inst_res.exception);
            if(is_fatal(cur_inst_res.exception)){
                fprintf(stdout, "ERROR: %s\n", Exceptions[cur_inst_res.exception]);
                break;
            }
            cur_inst = 0;
        }
        cur_inst = cur_inst_res.value;
        cpu.pc += 4;
        cpu.reg[0] = 0;
        inst parsed_inst = decode(cur_inst);
        // debug mode
        if(argc == 3 && argv[2][0] == '-' && argv[2][1] == 'd'){
            printf("-------------\n");
            printf("pc:0x%x\n", cpu.pc);
            printf("inst:0x%x\n",cur_inst);
            printf("opcode:0x%x | ",parsed_inst.opcode);
            printf("rd:%u | ",parsed_inst.rd);
            printf("rs1:%u | ",parsed_inst.rs1);
            printf("rs2:%u\n",parsed_inst.rs2);
            printf("imI:%d | ",parsed_inst.imI);
            printf("imS:%d | ",parsed_inst.imS);
            printf("imB:%d | ",parsed_inst.imB);
            printf("imU:%d | ",parsed_inst.imU);
            printf("imJ:%d\n",parsed_inst.imJ);
            printf("funct3:0x%x | ",parsed_inst.funct3);
            printf("funct7:0x%x | ",parsed_inst.funct7);
            printf("shamt5:0x%x | ",parsed_inst.shamt5);
            printf("shamt6:0x%x\n",parsed_inst.shamt6);
            dump_regs();
            char rep;
            scanf("%c",&rep);
        }
        cpu.reg[0] = 0;
        Result exec_inst_res = execute(parsed_inst);
        if(exec_inst_res.exception != NullException){
            take_trap(exec_inst_res.exception);
            if(is_fatal(exec_inst_res.exception)){
                fprintf(stderr, "ERROR: %s\n", Exceptions[exec_inst_res.exception]);
                break;
            }
        }
    }
    cpu.reg[0] = 0;
    dump_regs();
    dump_csrs();
    return 0;
}