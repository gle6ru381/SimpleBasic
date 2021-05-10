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
    int16_t literalVal;
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

Variable* newVarL(int8_t location, int16_t value, char varName)
{
    Variable* var = malloc(sizeof(*var));
    if (var == NULL)
        return var;
    var->varName = varName;
    var->location = location;
    var->isAccum = 0;
    var->isLiterall = 1;
    var->literalVal = value;
    return var;
}

typedef struct _Instruction {
    String* instruction;
    int16_t line;
    int8_t location;
    char isPromise;
    int16_t promiseData;
} Instruction;

Instruction* newInstruction(String* instruction, int8_t location)
{
    Instruction* inst = malloc(sizeof(*inst));
    if (inst == NULL)
        return inst;
    inst->instruction = instruction;
    inst->location = location;
    inst->isPromise = 0;
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
    char tempVarName;
    char lastLiteralName;
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
    context->tempVarName = 0;
    context->lastLiteralName = -127;

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
            context->variables[i]->isAccum = 0;
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

void addInstructionp(char* instruction, uint8_t line)
{
    if (context->instructions == NULL) {
        context->instructions = malloc(sizeof(Instruction) * 10);
        context->instrListCapacity = 10;
    }
    if (context->instrListSize == context->instrListCapacity) {
        context->instructions = realloc(context->instructions,
                                        sizeof(Instruction) * (context->instrListSize + 10));
        context->instrListCapacity += 10;
    }
    String* inst = newString1(instruction);

    char addrStr[4];
    uint8_t addr = context->instrucionStack++;
    addrStr[0] = (addr / 10) + '0';
    addrStr[1] = (addr % 10) + '0';
    addrStr[2] = ' ';
    addrStr[3] = '\0';

    strPrependc(inst, addrStr);


    Instruction* ins = newInstruction(inst, 0);
    ins->promiseData = line;
    ins->line = -1;
    ins->isPromise = 1;
    context->instructions[context->instrListSize++] = ins;
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

void storeAccum(char varName)
{
    Variable* var = findVar(varName, 1);
    addInstructiono("STORE", var->location);
}

int variableInAccum(char varName)
{
    Variable* var = findVar(varName, 0);
    if (var == NULL)
        return 1;
    return var->isAccum;
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
        Instruction* ins = context->instructions[i];
        if (ins->isPromise) {
           int8_t addr = instructionLocationByLine(ins->promiseData);
           char varStr[4];
           varStr[0] = ' ';
           varStr[1] = (addr / 10) + '0';
           varStr[2] = (addr % 10) + '0';
           varStr[3] = '\0';
           strAppendc(ins->instruction, varStr);
        }
        String* inst = ins->instruction;
        for (uint16_t j = 0; j < inst->length; j++) {
            fputc(inst->string[j], fd);
        }
        fputc('\n', fd);
    }
}

int pushTempVar()
{
    addVariable(context->tempVarName);
    return context->tempVarName++;
}

void popTempVar()
{
    context->tempVarName--;
    free(context->variables[--context->varListSize]);
    context->variableStack++;
}

int isTempVar(int varName)
{
    Variable* var = findVar(varName, 0);
    if (var->varName < 'A' && var->isLiterall == 0)
        return 1;
    else
        return 0;
}

#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdlib.h>

Variable* findLiteral(int16_t val)
{
    for (uint8_t i = 0; i < context->varListSize; i++) {
        if (context->variables[i]->isLiterall) {
            if (context->variables[i]->literalVal == val)
                return context->variables[i];
        }
    }
    return NULL;
}

int8_t getLiteralLocation(int16_t val)
{
    Variable* var = findLiteral(val);
    return var->location;
}

void addLiteral(int16_t val)
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
    if (findLiteral(val) == NULL) {
        if (context->variableStack == context->instrucionStack)
            return;
        String* str = newString1(" = ");
        char add[3];
        add[0] = context->variableStack / 10 + '0';
        add[1] = context->variableStack % 10 + '0';
        add[2] = '\0';

        char var[6];
        uint16_t pVal = (uint16_t)val;
        var[0] = '-';
        var[1] = pVal / 1000 + '0';
        var[2] = pVal / 100 % 10 + '0';
        var[3] = pVal / 10 % 10 + '0';
        var[4] = pVal % 10 + '0';
        var[5] = '\0';

        strPrependc(str, add);
        strAppendc(str, var);

        if (context->instructions == NULL) {
            context->instructions = malloc(sizeof(Instruction) * 10);
            context->instrListCapacity = 10;
        }
        if (context->instrListSize == context->instrListCapacity) {
            context->instructions = realloc(context->instructions,
                                            sizeof(Instruction) * (context->instrListSize + 10));
            context->instrListCapacity += 10;
        }
        context->instructions[context->instrListSize++] = newInstruction(str, context->variableStack);

        context->variables[context->varListSize++] = newVarL(context->variableStack--, val, context->lastLiteralName++);
        return;
    }
    return;
}

char getLiteralName(int16_t val)
{
    Variable* var = findLiteral(val);
    return var->varName;
}

void findLiterals(char* str)
{
    for (uint32_t i = 0; i < strlen(str); i++)
    {
        if (isupper(str[i])) {
            if (isupper(str[i + 1]))
                return;
            else
                continue;
        } else if (isdigit(str[i])) {
            char number[7];
            uint8_t nIdx = 0;
            uint32_t j = i;
            for (; j < strlen(str); j++) {
                if (str[j] == ' ' || str[j] == '\0' || str[j] == '\n') {
                    break;
                }
                number[nIdx++] = str[j];
            }
            number[nIdx] = '\0';
            int16_t val = atoi(number);
            addLiteral(val);
            i = j + 1;
        }
    }
}
