#include "context.h"
#include <malloc.h>
#include "mystring.h"

#define MEMORY_SIZE 100

int16_t currentLine;

typedef struct _Variable {
    char varName;
    int8_t location;
    int8_t isAccum;
    int8_t isLiterall;
} Variable;

Variable* newVar1(char varName)
{
    Variable* var = malloc(sizeof(*var));
    if (var == NULL)
        return var;
    var->varName = varName;
    var->isLiterall = 0;
    var->location = UNKNOWN_LOCATION;
    var->isAccum = 0;
    return var;
}

Variable* newVar2(char varName, int8_t location)
{
    Variable* var = newVar1(varName);
    var->location = location;
    return var;
}

typedef struct _Instruction {
    String* instruction;
    int16_t line;
    int8_t location;
} Instruction;

Instruction* newInstruction(String* instruction, int8_t location)
{
    Instruction* inst = malloc(sizeof(*inst));
    if (inst == NULL)
        return inst;
    inst->instruction = instruction;
    inst->location = location;
    inst->line = currentLine;
    return inst;
}

typedef struct _Context {
    Variable** variables;
    Instruction** instructions;
    uint8_t varListSize;
    uint8_t varListCapacity;
    uint8_t instrListSize;
    uint8_t instrListCapacity;
    uint8_t instrucionStack;
    uint8_t variableStack;
} Context;

static Context* context;

void initContext()
{
    context = malloc(sizeof (Context));
    context->variables = NULL;
    context->instructions = NULL;
    context->instrucionStack = 0;
    context->variableStack = MEMORY_SIZE - 1;
    context->varListSize = 0;
    context->instrListSize = 0;
    context->varListCapacity = 0;
    context->instrListCapacity = 0;

    currentLine = -1;
}

int setCurrentLine(int16_t line)
{
    if (line <= currentLine) {
        return -1;
    }
    currentLine = line;
    return 0;
}

static Variable* findVar(char varName, char storeAccum)
{
//    if (context->variables == NULL)
//        return NULL;

    for (uint8_t i = 0; i < context->varListSize; i++) {
        if (storeAccum) {
            if (context->variables[i]->isAccum) {
                context->variables[i]->isAccum = 0;
            }
        }
        if (context->variables[i]->varName == varName) {
            return context->variables[i];
        }
    }
    return NULL;
}

int addVariable(char varName)
{
    if (context->variables == NULL) {
        context->variables = malloc(sizeof (Variable*) * 10);
        context->varListCapacity = 10;
    }
    if (context->varListSize == context->varListCapacity) {
        context->variables = realloc(context->variables,
                                     sizeof(Variable) * (context->varListSize + 10));
        context->varListCapacity += 10;
    }
    if (findVar(varName, 0) == NULL) {
        if (context->variableStack == context->instrucionStack)
            return -2;
        context->variables[context->varListSize++] = newVar2(varName, context->variableStack--);
        return 0;
    }
    return -1;
}

//uint16_t getVariable(char varName)
//{
//    Variable* var = findVar(varName);
//    if (var == NULL)
//        return ~0;
//    return var->data;
//}

int8_t getVariableLocation(char varName)
{
    Variable* var = findVar(varName, 0);
    if (var == NULL)
        return UNKNOWN_LOCATION;
    return var->location;
}

void addInstructionv(char* instruction, char varName)
{
    Variable* var = findVar(varName, 0);
    addInstructiono(instruction, var->location);
//    String* instStr = newString1(instruction);
//    char addrStr[4];
//    char varStr[4];
//    uint8_t addr = context->instrucionStack++;
//    addrStr[0] = addr / 10;
//    addrStr[1] = addr % 10;
//    addrStr[2] = ' ';
//    addrStr[3] = '\0';

//    varStr[0] = ' ';
//    varStr[1] = var->location / 10;
//    varStr[2] = var->location % 10;
//    varStr[3] = '\0';

//    instStr = strPrependc(instStr, addrStr);
//    instStr = strPrependc(instStr, varStr);
//    context->instructions[context->instrListSize++] = newInstruction(instStr);
}

void addInstructiono(char* instruction, int8_t operand)
{
    String* instStr = newString1(instruction);
    char addrStr[4];
    char varStr[4];
    uint8_t addr = context->instrucionStack++;
    addrStr[0] = (addr / 10) + '0';
    addrStr[1] = (addr % 10) + '0';
    addrStr[2] = ' ';
    addrStr[3] = '\0';

    varStr[0] = ' ';
    varStr[1] = (operand / 10) + '0';
    varStr[2] = (operand % 10) + '0';
    varStr[3] = '\0';

    instStr = strPrependc(instStr, addrStr);
    instStr = strAppendc(instStr, varStr);
    if (context->instructions == NULL) {
        context->instructions = malloc(sizeof(Instruction) * 10);
        context->instrListCapacity = 10;
    }
    if (context->instrListSize == context->instrListCapacity) {
        context->instructions = realloc(context->instructions,
                                        sizeof(Instruction) * (context->instrListSize + 10));
        context->instrListCapacity += 10;
    }
    context->instructions[context->instrListSize++] = newInstruction(instStr, addr);
}

void addInstructionj(char* instruction, int offset)
{
    addInstructiono(instruction, context->instrucionStack + offset);
}

void addInstructionEnd()
{
    uint8_t addr = context->instrucionStack;
    char addrStr[4];
    addrStr[0] = (addr / 10) + '0';
    addrStr[1] = (addr % 10) + '0';
    addrStr[2] = ' ';
    addrStr[3] = '\0';
    String* instStr = newString1(addrStr);
    instStr = strAppendc(instStr, "HALT");

    if (context->instructions == NULL) {
        context->instructions = malloc(sizeof(Instruction) * 10);
        context->instrListCapacity = 10;
    }
    if (context->instrListSize == context->instrListCapacity) {
        context->instructions = realloc(context->instructions,
                                        sizeof(Instruction) * (context->instrListSize + 10));
        context->instrListCapacity += 10;
    }
    context->instructions[context->instrListSize++] = newInstruction(instStr, addr);
}

void moveAccum(char varName)
{
    Variable* var = findVar(varName, 1);
    addInstructiono("LOAD", var->location);
    var->isAccum = 1;
}

void storeAccum()
{
    for (int8_t i = 0; i < context->varListSize; i++) {
        Variable* var = context->variables[i];
        if (var->isAccum) {
            addInstructiono("STORE", var->location);
            return;
        }
    }
}

int variableInAccum(char varName)
{
    return findVar(varName, 0)->isAccum;
}

int8_t lastInstructionLocation()
{
    return context->instructions[context->instrListSize - 1]->location;
}

int8_t instructionLocationByLine(int16_t line)
{
    for (uint8_t i = 0; i < context->instrListSize; i++) {
        if (context->instructions[i]->line == line)
            return context->instructions[i]->location;
    }
    return UNKNOWN_LOCATION;
}

void writeInstruction(FILE* fd)
{
    for (uint8_t i = 0; i < context->instrListSize; i++) {
        String* inst = context->instructions[i]->instruction;
        for (uint16_t j = 0; j < inst->length; j++) {
            fputc(inst->string[j], fd);
        }
        fputc('\n', fd);
    }
}
