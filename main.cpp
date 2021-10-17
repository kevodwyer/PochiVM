#include <string>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <vector>
#include "runtime/tutorial101.h"

#include "pochivm.h"

using namespace PochiVM;          // All PochiVM APIs live in this namespace
using FnPrototype = void(*)(Tutorial101, uint8_t*, uint8_t*);

const int MAX_FUNCTIONS = 10;

static int indexToHash[MAX_FUNCTIONS];
static int indexToFreq[MAX_FUNCTIONS];
static std::array<std::vector<int>, MAX_FUNCTIONS> compiledOpCodes;

static int numberOfCompiledFunctions = 0;
//hash=1880050 index=1 [0 4], [1 11], [2 6]
//static bool allowedOpcodes[16] = {false,false,false,false,true,false,true,false,false,false,false,true,false,false,false,false};
//hash=748804039 index=0 [0 4], [1 15], [2 14], [3 7], [4 12], [5 6]
static bool allowedOpcodes[16] = {false,false,false,false,true,false,true,true,false,false,false,false,true,false,true,true};
//default
//static bool allowedOpcodes[16] = {false,false,false,false,true,false,true,true,false,true,true,true,true,true,true,true};
//none
//static bool allowedOpcodes[16] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};


void incrementFrequency(int index) {
    if (index >=0) {
        indexToFreq[index] = indexToFreq[index] + 1;
    }
}
int getFrequency(int index) {
    if (index < 0) {
        return -1;
    }
    return indexToFreq[index];
}

int setIndex(int hash) {
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        if (indexToHash[i] == -1) {
            indexToHash[i] = hash;
            return i;
        }
    }
    return -1;
}
int getIndex(int hash) {
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        if (indexToHash[i] == hash) {
            return i;
        }
    }
    return setIndex(hash);
}

int generateHash(uint8_t* ops, int opCodesLength) {
    int hash = 37;
    for (int i = 0; i < opCodesLength; i++) {
        hash = hash * 37 + ops[i];
    }
    return hash;
}

void arraycopy(uint8_t* src, int fromIndex, uint8_t* dest, int destIndex, int length) {
    for (int i = 0; i < length; i++) {
        dest[destIndex++] = src[fromIndex++];
    }
}

void init(uint8_t* memory) {
    const int loopLength = 14;
    const int jumpLength = 10;
    const uint8_t b = 64;

    uint8_t loop[loopLength] = { LOAD_SP, b, STORE_1, LOAD_SP, b,
            LOAD_EBX_MEM, LOAD_SP, (b + 1), LOAD_EAX_MEM, XOR, DIV , LOAD_EBX_MEM, ADD, STORE_EAX };
    uint8_t jump[10] = { LOAD_SP, b, LOAD_EAX_MEM, INC, STORE_EAX, LOAD_SP, b + 2,
                LOAD_EBX_MEM, BNE, 3 };
    uint8_t jump2[10] = { LOAD_SP, b + 3, LOAD_EAX_MEM, INC,
                STORE_EAX, LOAD_SP, b + 4,
                LOAD_EBX_MEM, BNE, 0 };
    uint8_t jump3[10] = { LOAD_SP, b + 5, LOAD_EAX_MEM, INC,
                STORE_EAX, LOAD_SP, b + 6,
                LOAD_EBX_MEM, BNE, 0 };
    uint8_t jump4[10] = { LOAD_SP, b + 7, LOAD_EAX_MEM, INC,
                STORE_EAX, LOAD_SP, b + 8,
                LOAD_EBX_MEM, BNE, 0 };

    //	let stack = [1, 0, 11, 1, 11, 1, 11, 1, 11];
    const int stackLength = 9;
    //char stack[stackLength] = {1, 0, 5, 1, 5, 1, 5, 1, 2};
    //char stack[stackLength] = {1, 0, 11, 1, 11, 1, 11, 1, 11};
    uint8_t stack[stackLength] = { 1, 0, 11, 1, 101, 1, 101, 1, 101 };
    arraycopy(loop, 0, memory, 0, 14);
    arraycopy(jump, 0, memory, loopLength, jumpLength);
    arraycopy(jump2, 0, memory, loopLength + jumpLength, jumpLength);
    arraycopy(jump3, 0, memory, loopLength + 2 * jumpLength, jumpLength);
    arraycopy(jump4, 0, memory, loopLength + 3 * jumpLength, jumpLength);
    arraycopy(stack, 0, memory, b, stackLength);

    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        indexToHash[i] = -1;
        indexToFreq[i] = 0;
    }
}

void mainLoop( uint8_t* registers,  uint8_t* memory) {
    Tutorial101 computer;
    uint8_t opCodes[10] = {0};
    int opCodesLength = 0;
    uint8_t offset = registers[IP];
    int compiledCalls = 0;

    // Initialize PochiVM global contexts for this thread
    // The contexts are automatically destructed on destruction of these variables
    AutoThreadPochiVMContext apv;
    AutoThreadErrorContext arc;
    AutoThreadLLVMCodegenContext alc;

    NewModule("test");
    while (true) {
        uint8_t op = memory[offset++];
        if (allowedOpcodes[op]) {
            opCodes[opCodesLength++] = op;
        }
        else {
            if (opCodesLength >= 2) {
                int hash = generateHash(opCodes, opCodesLength);
                int index = getIndex(hash);
                incrementFrequency(index);
                int freq = getFrequency(index);
                if (freq > 100 && numberOfCompiledFunctions == 1) {
                    compiledCalls++;
                    std::string fnName = "call_cpp_fn";
                    fnName += std::to_string(index);
                    FastInterpFunction<FnPrototype> interpFn = thread_pochiVMContext->m_curModule->GetFastInterpGeneratedFunction<FnPrototype>(fnName);
                    interpFn(computer, registers, memory);
                    opCodesLength = 0;
                    if(!computer.decodeOp(op, registers, memory)) {
                        break;
                    }
                    offset = registers[IP];
                    continue;
                }
                else if (freq == 100) {
                    printf("\nhash=%d index=%d", hash, index);
                    std::vector<int> ops;
                    for (int j = 0; j < opCodesLength; j++) {
                        printf("\nfound [%d %d]", j, opCodes[j]);
                        ops.push_back(opCodes[j]);
                    }
                    compiledOpCodes[static_cast<unsigned long>(index)] = ops;
                    numberOfCompiledFunctions++;
                    if (numberOfCompiledFunctions == 1) {
                        /*
                        for(unsigned long i=0; i < 2; i++) {
                            std::vector<int> currOps = compiledOpCodes[i];
                            std::string fnName = "call_cpp_fn";
                            fnName += std::to_string(i);
                            auto [fn, comp, regs, mem] = NewFunction<FnPrototype>(fnName);
                            fn.SetBody();
                            for (auto& currOp: currOps) {
                                printf("\ngen [%d] ", currOp);
                                if (op ==LOAD_EAX_MEM) { //4
                                    fn.GetBody().Append(comp.opLOAD_EAX_MEM(regs, mem));
                                } else if(op == STORE_EAX) { //6
                                    fn.GetBody().Append(comp.opSTORE_EAX(regs, mem));
                                } else if(op == LOAD_EBX_MEM) { //7
                                    fn.GetBody().Append(comp.opLOAD_EBX_MEM(regs, mem));
                                } else if(op == STORE_EBX) { //9
                                    fn.GetBody().Append(comp.opSTORE_EBX(regs, mem));
                                } else if(op == STORE_1) { //10
                                    fn.GetBody().Append(comp.opSTORE_1(regs, mem));
                                } else if(op == INC) { //11
                                    fn.GetBody().Append(comp.opINC(regs));
                                } else if(op == ADD) { //12
                                    fn.GetBody().Append(comp.opADD(regs));
                                } else if(op == MUL) { //13
                                    fn.GetBody().Append(comp.opMUL(regs));
                                } else if(op == DIV) { //14
                                    fn.GetBody().Append(comp.opDIV(regs));
                                } else if(op == XOR) { //15
                                    fn.GetBody().Append(comp.opXOR(regs));
                                }
                            }
                        }*/
                        std::vector<int> currOps = compiledOpCodes[0];
                        std::string fnName = "call_cpp_fn";
                        fnName += std::to_string(0);
                        auto [fn, comp, regs, mem] = NewFunction<FnPrototype>(fnName);
                        fn.SetBody();
                        //hash=1880050 index=1 [0 4], [1 11], [2 6]
                        //fn.GetBody().Append(comp.opLOAD_EAX_MEM(regs, mem));
                        //fn.GetBody().Append(comp.opINC(regs));
                        //fn.GetBody().Append(comp.opSTORE_EAX(regs, mem));

                        //hash=748804039 index=0 [0 4], [1 15], [2 14], [3 7], [4 12], [5 6]
                        fn.GetBody().Append(comp.opLOAD_EAX_MEM(regs, mem));
                        fn.GetBody().Append(comp.opXOR(regs));
                        fn.GetBody().Append(comp.opDIV(regs));
                        fn.GetBody().Append(comp.opLOAD_EBX_MEM(regs, mem));
                        fn.GetBody().Append(comp.opADD(regs));
                        fn.GetBody().Append(comp.opSTORE_EAX(regs, mem));

                        ReleaseAssert(thread_pochiVMContext->m_curModule->Validate());
                        thread_pochiVMContext->m_curModule->PrepareForFastInterp();
                        printf("\ndone-compiling");
                    }
                }
            }
            if (!computer.decodeOps(opCodes, opCodesLength, registers, memory)) {
                break;
            }
            opCodesLength = 0;
            if (!computer.decodeOp(op, registers, memory)) {
                break;
            }
            offset = registers[IP];
        }
    }
    printf("\nopCounter= %d compiled calls=%d", computer.opCounter, compiledCalls);

}
int main(){
    //uint8_t registers[4] = {0,0,0,0};
    uint8_t* registers = new uint8_t[static_cast<size_t>(4)];
    memset(registers, 0, sizeof(uint8_t) * static_cast<size_t>(4));
    //uint8_t memory[128] = {0};
    uint8_t* memory = new uint8_t[static_cast<size_t>(128)];
    memset(memory, 0, sizeof(uint8_t) * static_cast<size_t>(128));

    init(memory);
    mainLoop(registers, memory);

    printf("\ndone\n");
    return 0;
}
