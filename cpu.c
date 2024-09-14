#include "cpu.h"

CPU cpu;

void initCPU(){
    cpu.reg[2] = DRAM_SIZE;
    cpu.pc = 0;
}

inst decode(u32 ins){
    inst cur_inst;
    cur_inst.opcode = ins & 0x7f;
    cur_inst.rd = (ins>>7) & 0x1f;
    cur_inst.rs1 = (ins>>15) & 0x1f;
    cur_inst.rs2 = (ins>>20) & 0x1f;
    cur_inst.im5 = (ins>>7) & 0x1f; //7-11 bits
    cur_inst.im7 = (ins>>25) & 0x7f; //25-31 bits
    cur_inst.im12 = (ins>>20) & 0xfff; //20-31 bits
    cur_inst.im20 = (ins>>12) & 0xfffff; //12-31 bits
    return cur_inst;
}

u32 fetch(){
    u32 ins = cpu.dram[cpu.pc];
    if(ins == 0){
        fprintf(stderr, "ERROR: Instruction couldn't be fetched!");
        exit(-1);
    }
    #ifdef LITTLE_ENDIAN
        ins = ((ins&0xff) << 24) | (((ins>>8)&0xff) << 16) | (((ins>>16)&0xff) << 8) | ((ins>>24)&0xff);
    #endif
    return ins;
}

void execute(inst ins){
    u64 addr;
    u64 val;
    u32 shamt;
    u64 imm;
    
    switch(ins.opcode) {
         case 0x33: {
            // R-type instructions (e.g., add, sub, mul, etc.)
            shamt = cpu.reg[ins.rs2] & 0x3f; // Only use the lower 6 bits for shifts
            switch (ins.funct3) {
                case 0x0:  // ADD, SUB, MUL
                    if (ins.funct7 == 0x00) {
                        cpu.reg[ins.rd] = cpu.reg[ins.rs1] + cpu.reg[ins.rs2]; // add
                    } else if (ins.funct7 == 0x01) {
                        cpu.reg[ins.rd] = cpu.reg[ins.rs1] * cpu.reg[ins.rs2]; // mul
                    } else if (ins.funct7 == 0x20) {
                        cpu.reg[ins.rd] = cpu.reg[ins.rs1] - cpu.reg[ins.rs2]; // sub
                    }
                    break;
                case 0x1:  // SLL
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] << shamt; // sll
                    break;
                case 0x2:  // SLT
                    cpu.reg[ins.rd] = (int64_t)cpu.reg[ins.rs1] < (int64_t)cpu.reg[ins.rs2] ? 1 : 0; // slt
                    break;
                case 0x3:  // SLTU
                    cpu.reg[ins.rd] = (uint64_t)cpu.reg[ins.rs1] < (uint64_t)cpu.reg[ins.rs2] ? 1 : 0; // sltu
                    break;
                case 0x4:  // XOR
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] ^ cpu.reg[ins.rs2]; // xor
                    break;
                case 0x5:
                    if (ins.funct7 == 0x00) {
                        cpu.reg[ins.rd] = cpu.reg[ins.rs1] >> shamt; // srl
                    } else if (ins.funct7 == 0x20) {
                        cpu.reg[ins.rd] = (int64_t)cpu.reg[ins.rs1] >> shamt; // sra
                    }
                    break;
                case 0x6:  // OR
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] | cpu.reg[ins.rs2]; // or
                    break;
                case 0x7:  // AND
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] & cpu.reg[ins.rs2]; // and
                    break;
                default:
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet! Opcode: 0x33,funct3: 0x%x, funct7: 0x%x\n", ins.funct3, ins.funct7);
                    exit(-1);
            }
            break;
        }

        case 0x23: {
            // S-type instructions (e.g., store)
            imm = ((ins.im7 << 5) | ins.im5); // Combine im7 and im5 into a single immediate value
            addr = cpu.reg[ins.rs1] + imm;
            switch (ins.funct3) {
                case 0x0:
                    store(addr, 8, cpu.reg[ins.rs2]);  // sb (store byte)
                    break;
                case 0x1:
                    store(addr, 16, cpu.reg[ins.rs2]);  // sh (store halfword)
                    break;
                case 0x2:
                    store(addr, 32, cpu.reg[ins.rs2]);  // sw (store word)
                    break;
                case 0x3:
                    store(addr, 64, cpu.reg[ins.rs2]);  // sd (store doubleword)
                    break;
                default:
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet! Opcode: 0x23, funct3: 0x%x, funct7: 0x%x\n", ins.funct3, ins.funct7);
                    exit(-1);
            }
            break;
        }


         case 0x13:  // I-type instructions (e.g., addi)
            shamt = ins.im12 & 0x3f;  // For shift instructions
            switch (ins.funct3) {
                case 0x0:  // addi
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] + ins.im12;
                    break;
                case 0x1:  // slli
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] << shamt;  // Logical left shift
                    break;
                case 0x2:  // slti
                    cpu.reg[ins.rd] = (int64_t)cpu.reg[ins.rs1] < ins.im12 ? 1 : 0;
                    break;
                case 0x3:  // sltiu
                    cpu.reg[ins.rd] = (uint64_t)cpu.reg[ins.rs1] < (uint64_t)ins.im12 ? 1 : 0;
                    break;
                case 0x4:  // xori
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] ^ ins.im12;
                    break;
                case 0x5:
                    if ((ins.funct7 >> 1) == 0x00) {
                        cpu.reg[ins.rd] = (uint64_t)cpu.reg[ins.rs1] >> shamt;  // srli (Logical right shift)
                    } else if ((ins.funct7 >> 1) == 0x10) {
                        cpu.reg[ins.rd] = (int64_t)cpu.reg[ins.rs1] >> shamt;  // srai (Arithmetic right shift)
                    } else{
                        fprintf(stderr, "ERROR: Invalid funct7 for shift operation");
                            exit(-1);
                    }
                    break;
                case 0x6:  // ori
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] | ins.im12;
                    break;
                case 0x7:  // andi
                    cpu.reg[ins.rd] = cpu.reg[ins.rs1] & ins.im12;
                    break;
                default:
                   fprintf(stderr, "ERROR: Operation hasn't been implemented yet! Opcode: 0x13, funct3: 0x%x, funct7: 0x%x\n", ins.funct3, ins.funct7);
                   exit(-1);
                    
            }
            break;
       
         case 0x03:
            // Load instructions
            addr = cpu.reg[ins.rs1] + ins.im12;
            switch (ins.funct3) { // funct3
                case 0x0: {
                    // lb
                    val = load(addr, 8);
                    cpu.reg[ins.rd] = (int8_t)val;
                    break;
                }
                case 0x1: {
                    // lh
                    val = load(addr, 16);
                    cpu.reg[ins.rd] = (int16_t)val;
                    break;
                }
                case 0x2: {
                    // lw
                    val = load(addr, 32);
                    cpu.reg[ins.rd] = (int32_t)val;
                    break;
                }
                case 0x3: {
                    // ld
                    val = load(addr, 64);
                    cpu.reg[ins.rd] = val;
                    break;
                }
                case 0x4: {
                    // lbu
                    val = load(addr, 8);
                    cpu.reg[ins.rd] = (uint8_t)val;
                    break;
                }
                case 0x5: {
                    // lhu
                    val = load(addr, 16);
                    cpu.reg[ins.rd] = (uint16_t)val;
                    break;
                }
                case 0x6: {
                    // lwu
                    val = load(addr, 32);
                    cpu.reg[ins.rd] = (uint32_t)val;
                    break;
                }
                default:
                    
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet! Opcode: 0x03, funct3: 0x%x, funct7: 0x%x\n", ins.funct3, ins.funct7);
                    exit(-1);
            }
            break;
        case 0x17:  // AUIPC
            cpu.reg[ins.rd] = cpu.pc + ins.im20 - 4;  // AUIPC adds PC + 20-bit immediate shifted left by 12
            break;

        case 0x1b:  // ADDIW, SLLIW, SRLIW, SRAIW
            imm = (int64_t)((int32_t)(ins.im12));  // Sign-extend 12-bit immediate to 64 bits
            shamt = ins.im12 & 0x1f;  // Extract lower 5 bits for shift amount
            switch (ins.funct3) {
                case 0x0:  // ADDIW
                    cpu.reg[ins.rd] = (int32_t)(cpu.reg[ins.rs1] + imm);  // Perform ADDIW, truncate result to 32-bit
                    break;
                case 0x1:  // SLLIW
                    cpu.reg[ins.rd] = (int32_t)(cpu.reg[ins.rs1] << shamt);  // Logical shift left and truncate to 32-bit
                    break;
                case 0x5:
                    if (ins.funct7 == 0x00) {  // SRLIW
                        cpu.reg[ins.rd] = (int32_t)((uint32_t)cpu.reg[ins.rs1] >> shamt);
                    } else if (ins.funct7 == 0x20) {  // SRAIW (Arithmetic shift right)
                        cpu.reg[ins.rd] = (int32_t)((int32_t)cpu.reg[ins.rs1] >> shamt);
                    } else {
                        fprintf(stderr, "ERROR: Invalid funct7 for SRAIW/SRLIW");
                        exit(-1);
                    }
                    break;
                default:
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet! Opcode: 0x1B, funct3: 0x%x, funct7: 0x%x\n", ins.funct3, ins.funct7);
                    exit(-1);
            }
            break;

        
        case 0x37: {
            // LUI (Load Upper Immediate)
            int32_t imm = (inst & 0xfffff000);  // Extract upper 20 bits
            cpu.reg[rd] = (uint64_t)(int64_t)imm;  // Sign-extend and store in 64-bit register
            break;
        }

        case 0x3B: {
            // RV64I instructions (e.g., addw, subw, sllw, etc.)
            shamt = cpu.reg[ins.rs2] & 0x1f; // Only use the lower 5 bits for shifts
            switch (ins.funct3) {
                case 0x0:  // ADDW, SUBW
                    if (ins.funct7 == 0x00) {
                        cpu.reg[ins.rd] = (int32_t)(cpu.reg[ins.rs1] + cpu.reg[ins.rs2]); // addw
                    } else if (ins.funct7 == 0x20) {
                        cpu.reg[ins.rd] = (int32_t)(cpu.reg[ins.rs1] - cpu.reg[ins.rs2]); // subw
                    }
                    break;
                case 0x1:  // SLLW
                    cpu.reg[ins.rd] = (int32_t)(cpu.reg[ins.rs1] << shamt); // sllw
                    break;
                case 0x5:
                    if (ins.funct7 == 0x00) {
                        cpu.reg[ins.rd] = (int32_t)(cpu.reg[ins.rs1] >> shamt); // srlw
                    } else if (ins.funct7 == 0x20) {
                        cpu.reg[ins.rd] = (int32_t)((int32_t)cpu.reg[ins.rs1] >> shamt); // sraw
                    }
                    break;
                default:
                    fprintf(stderr, "ERROR: Operation hasn't been implemented yet! Opcode: 0x3B, funct3: 0x%x, funct7: 0x%x\n", ins.funct3, ins.funct7);
                    exit(-1);
            }
            break;
        }


    case 0x63: {
        // BEQ, BNE, BLT, BGE, BLTU, BGEU (Branch Instructions)
        uint64_t imm = (((inst & 0x80000000) >> 19) // imm[12]
                        | ((inst & 0x80) << 4)    // imm[11]
                        | ((inst >> 20) & 0x7e0) // imm[10:5]
                        | ((inst >> 7) & 0x1e)); // imm[4:1]

        switch (ins.funct3) {
            case 0x0: // BEQ
                if (cpu.reg[ins.rs1] == cpu.reg[ins.rs2]) {
                    cpu.pc = (cpu.pc + imm - 4) & 0xFFFFFFFF; // Wrap around address
                }
                break;

            case 0x1: // BNE
                if (cpu.reg[ins.rs1] != cpu.reg[ins.rs2]) {
                    cpu.pc = (cpu.pc + imm - 4) & 0xFFFFFFFF; // Wrap around address
                }
                break;

            case 0x4: // BLT
                if ((int64_t)cpu.reg[ins.rs1] < (int64_t)cpu.reg[ins.rs2]) {
                    cpu.pc = (cpu.pc + imm - 4) & 0xFFFFFFFF; // Wrap around address
                }
                break;

            case 0x5: // BGE
                if ((int64_t)cpu.reg[ins.rs1] >= (int64_t)cpu.reg[ins.rs2]) {
                    cpu.pc = (cpu.pc + imm - 4) & 0xFFFFFFFF; // Wrap around address
                }
                break;

            case 0x6: // BLTU
                if (cpu.reg[ins.rs1] < cpu.reg[ins.rs2]) {
                    cpu.pc = (cpu.pc + imm - 4) & 0xFFFFFFFF; // Wrap around address
                }
                break;

            case 0x7: // BGEU
                if (cpu.reg[ins.rs1] >= cpu.reg[ins.rs2]) {
                    cpu.pc = (cpu.pc + imm - 4) & 0xFFFFFFFF; // Wrap around address
                }
                break;

            default:
                fprintf(stderr, "ERROR: Branch instruction with funct3 %d not implemented yet!\n", ins.funct3);
                exit(-1);
        }
        break;

    }

    case 0x67: {
        // JALR (Jump and Link Register)
        uint64_t imm = ((int32_t)(inst & 0xfff00000) >> 20);
        uint64_t addr = (cpu.reg[ins.rs1] + imm) & ~1; // Ensure address is aligned
        cpu.reg[ins.rd] = cpu.pc;
        cpu.pc = addr;
        break;
    }

    case 0x6f: {
        // JAL (Jump and Link)
        cpu.reg[ins.rd] = cpu.pc;
        uint64_t imm = (((inst & 0x80000000) >> 11)    // imm[20]
                        | (inst & 0xff000)          // imm[19:12]
                        | ((inst >> 9) & 0x800)    // imm[11]
                        | ((inst >> 20) & 0x7fe)); // imm[10:1]
        cpu.pc = (cpu.pc + imm - 4) & 0xFFFFFFFF; // Wrap around address
        break;
    }


        default:
            fprintf(stderr, "ERROR: Operation with opcode %d not implemented yet!\n", ins.opcode);
            exit(-1);
    }
}
