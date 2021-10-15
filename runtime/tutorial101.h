#pragma once
#include <cstdio>
#include <stdlib.h>

typedef __uint8_t uint8_t;

const uint8_t HALT = 0;
const uint8_t JUMP = 1;
const uint8_t BNE = 2;
const uint8_t LOAD_SP = 3;
const uint8_t LOAD_EAX_MEM = 4;
const uint8_t LOAD_EAX_IB = 5;
const uint8_t STORE_EAX = 6;
const uint8_t LOAD_EBX_MEM = 7;
const uint8_t LOAD_EBX_IB = 8;
const uint8_t STORE_EBX = 9;
const uint8_t STORE_1 = 10;
const uint8_t INC = 11;
const uint8_t ADD = 12;
const uint8_t MUL = 13;
const uint8_t DIV = 14;
const uint8_t XOR = 15;

const int IP = 0;
const int EAX = 1;
const int EBX = 2;
const int SP = 3;

class Tutorial101
{
public:
    Tutorial101() : m_x(123), opCounter(0){}
    void Increment(int value);
    int Print() {
        //printf("m_x has value %d\n", m_x);
        return m_x;
    }
    void opJUMP(uint8_t* registers, uint8_t* memory);
    void opBNE(uint8_t* registers, uint8_t* memory);
    void opLOAD_SP(uint8_t* registers, uint8_t* memory);
    void opLOAD_EAX_MEM(uint8_t* registers, uint8_t* memory);
    void opLOAD_EAX_IB(uint8_t* registers, uint8_t* memory);
    void opSTORE_EAX(uint8_t* registers, uint8_t* memory);
    void opLOAD_EBX_MEM(uint8_t* registers, uint8_t* memory);
    void opLOAD_EBX_IB(uint8_t* registers, uint8_t* memory);
    void opSTORE_EBX(uint8_t* registers, uint8_t* memory);
    void opSTORE_1(uint8_t* registers, uint8_t* memory);
    void opINC(uint8_t* registers);
    void opADD(uint8_t* registers);
    void opMUL(uint8_t* registers);
    void opDIV(uint8_t* registers);
    void opXOR(uint8_t* registers);
    bool decodeOp(uint8_t op, uint8_t* registers, uint8_t* memory);
    bool decodeOps(uint8_t* ops, int opCodesLength, uint8_t* registers, uint8_t* memory);
    int m_x;
    int opCounter;
};
