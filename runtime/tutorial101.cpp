#include "tutorial101.h"



void Tutorial101::Increment(int value) {
    m_x += value;
}

void Tutorial101::opJUMP(uint8_t* registers, uint8_t* memory) {
    registers[IP] = memory[registers[IP] + 1];
}
void Tutorial101::opBNE(uint8_t* registers, uint8_t* memory) {
    if (registers[EAX] != registers[EBX])
        registers[IP] = memory[registers[IP] + 1];
    else
        registers[IP] += 2;
}
void Tutorial101::opLOAD_SP(uint8_t* registers, uint8_t* memory) {
    registers[SP] = memory[registers[IP] + 1];
    registers[IP] += 2;
}
void Tutorial101::opLOAD_EAX_MEM(uint8_t* registers, uint8_t* memory) {
    registers[EAX] = memory[registers[SP]];
    registers[IP]++;
}
void Tutorial101::opLOAD_EAX_IB(uint8_t* registers, uint8_t* memory) {
    registers[EAX] = memory[registers[IP] + 1];
    registers[IP] += 2;
}
void Tutorial101::opSTORE_EAX(uint8_t* registers, uint8_t* memory) {
    memory[registers[SP]] = registers[EAX];
    registers[IP]++;
}
void Tutorial101::opLOAD_EBX_MEM(uint8_t* registers, uint8_t* memory) {
    registers[EBX] = memory[registers[SP]];
    registers[IP]++;
}
void Tutorial101::opLOAD_EBX_IB(uint8_t* registers, uint8_t* memory) {
    registers[EBX] = memory[registers[IP] + 1];
    registers[IP] += 2;
}
void Tutorial101::opSTORE_EBX(uint8_t* registers, uint8_t* memory) {
    memory[registers[SP]] = registers[EBX];
    registers[IP]++;
}
void Tutorial101::opSTORE_1(uint8_t* registers, uint8_t* memory) {
    memory[registers[SP]] = 1;
    registers[IP]++;
}
void Tutorial101::opINC(uint8_t* registers) {
    registers[EAX]++;
    registers[IP]++;
}
void Tutorial101::opADD(uint8_t* registers) {
    registers[EAX] += registers[EBX];
    registers[IP]++;
}
void Tutorial101::opMUL(uint8_t* registers) {
    registers[EAX] *= registers[EBX];
    registers[IP]++;
}
void Tutorial101::opDIV(uint8_t* registers) {
    registers[EAX] /= registers[EBX];
    registers[IP]++;
}
void Tutorial101::opXOR(uint8_t* registers) {
    registers[EAX] ^= registers[EBX];
    registers[IP]++;
}
bool Tutorial101::decodeOp(uint8_t op, uint8_t* registers, uint8_t* memory) {
    opCounter++;
    if (op == HALT) {
        return false;
    }
    else if (op == JUMP) {
        opJUMP(registers, memory);
    }
    else if (op == BNE) {
        opBNE(registers, memory);
    }
    else if (op == LOAD_SP) {
        opLOAD_SP(registers, memory);
    }
    else if (op == LOAD_EAX_MEM) {
        opLOAD_EAX_MEM(registers, memory);
    }
    else if (op == LOAD_EAX_IB) {
        opLOAD_EAX_IB(registers, memory);
    }
    else if (op == STORE_EAX) {
        opSTORE_EAX(registers, memory);
    }
    else if (op == LOAD_EBX_MEM) {
        opLOAD_EBX_MEM(registers, memory);
    }
    else if (op == LOAD_EBX_IB) {
        opLOAD_EBX_IB(registers, memory);
    }
    else if (op == STORE_EBX) {
        opSTORE_EBX(registers, memory);
    }
    else if (op == STORE_1) {
        opSTORE_1(registers, memory);
    }
    else if (op == INC) {
        opINC(registers);
    }
    else if (op == ADD) {
        opADD(registers);
    }
    else if (op == MUL) {
        opMUL(registers);
    }
    else if (op == DIV) {
        opDIV(registers);
    }
    else if (op == XOR) {
        opXOR(registers);
    }
    else {
        printf("Illegal opcode: %d", op);
        return false;
    }
    return true;
}

bool Tutorial101::decodeOps(uint8_t* ops, int opCodesLength, uint8_t* registers, uint8_t* memory) {
    for (int i = 0; i < opCodesLength; i++) {
        if (!decodeOp(ops[i], registers, memory)) {
            return false;
        }
    }
    return true;
}
