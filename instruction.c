#include "instruction.h"
#include "mathTree.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

static char findVar(char* str)
{
    char var = '\0';
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ')
            continue;
        if (var != '\0') {
            var = '\0';
            break;
        }
        if (isupper(str[i])) {
            var = str[i];
        } else {
            var = '\0';
            break;
        }
    }
    return var;
}

int invokeInstr(char* instr, char* operand)
{

    if (!strcmp(instr, "INPUT")) {
        return genInputIns(operand);
    } else if (!strcmp(instr, "PRINT")) {
        return genPrintIns(operand);
    } else if (!strcmp(instr, "LET")) {
        return genLetIns(operand);
    } else if (!strcmp(instr, "IF")) {
        return genIfIns(operand);
    } else if (!strcmp(instr, "GOTO")) {
        return genGotoIns(operand);
    }
    return -1;
}

int genInputIns(char* ins)
{
    char var = findVar(ins);
    addVariable(var);
    addInstructionv("READ", var);
    return 0;
}

int genPrintIns(char* ins)
{
    char var = findVar(ins);
    addVariable(var);
    addInstructionv("WRITE", var);
    return 0;
}

int genLetIns(char* ins)
{
    uint16_t strLen = strlen(ins);
    char lVal = '\0';
    uint16_t i;
    for (i = 0; i < strLen; i++) {
        if (ins[i] == ' ')
            continue;
        if (isupper(ins[i])) {
            lVal = ins[i];
            break;
        } else if (ins[i] == '='){
            return -1;
        }
    }
    int8_t flag = 0;
    for (; i < strLen; i++) {
        if (ins[i] == ' ')
            continue;
        if (ins[i] == '=') {
            flag = 1;
            break;
        }
    }
    if (!flag)
        return -2;
    char rVal = evals(ins + i + 1);
    if (!variableInAccum(rVal)) {
        moveAccum(rVal);
    }
    addVariable(lVal);
    addInstructionv("STORE", lVal);
    return 0;
}

int genIfIns(char* ins)
{
    uint16_t strLen = strlen(ins);
    char lVal = '\0';
    uint16_t i;
    for (i = 0; i < strLen; i++) {
        if (ins[i] == ' ')
            continue;
        if (isupper(ins[i])) {
            lVal = ins[i];
            break;
        } else if (ins[i] == '='){
            return -1;
        }
    }
    int8_t flag = 0;
    for (; i < strLen && flag == 0; i++) {
        if (ins[i] == ' ')
            continue;
        switch (ins[i]) {
        case '!': {
            if (ins[i + 1] == '=')
                flag = 1;
            else
                return -3;
            break;
        }
        case '<':
            flag = 2;
            break;
        }
    }
    if (!flag)
        return -2;
    char buff[100];
    for (uint16_t j = 0; i < strLen; i++, j++) {
        if (isupper(ins[i]) && isupper(ins[i + 1])) {
            buff[j] = '\0';
            break;
        }
        buff[j] = ins[i];
    }
    char insIf[50] = {'\0'};
    for (uint16_t j = 0; i < strLen; i++, j++) {
        if (isupper(ins[i])) {
            insIf[j] = ins[i];
        } else {
            insIf[j] = '\0';
            break;
        }
    }

    char rVal = evals(buff);
    if (!variableInAccum(rVal)) {
        moveAccum(rVal);
    }
    if (flag == 1) {
        addInstructionv("SUB", lVal);
        addInstructionj("JZ");
        invokeInstr(insIf, &ins[i]);
    } else if (flag == 2) {
        addInstructionv("SUB", lVal);
        addInstructionj("JNEG");
        invokeInstr(insIf, &ins[i]);
    }
    return 0;
}

int genGotoIns(char* inst)
{
    char lineNum[4];
    uint8_t strLen = strlen(inst);
    uint8_t j = 0;
    for (uint8_t i = 0; i < strLen; i++) {
        if (inst[i] == ' ')
            continue;
        if (!isdigit(inst[i]))
            return -1;
        lineNum[j] = inst[i];
        j++;
    }
    lineNum[j] = '\0';
    uint8_t line = atoi(lineNum);
    int8_t location = instructionLocationByLine(line);
    if (location == UNKNOWN_LOCATION)
        return -2;
    addInstructiono("JUMP", location);
    return 0;
}
