#ifndef CONTEXT_H
#define CONTEXT_H
#include <inttypes.h>
#include <stdio.h>

#define UNKNOWN_LOCATION -1
#define ACCUM_LOCATION -2

void initContext();
int addVariable(char varName);
int8_t getVariableLocation(char varName);
int8_t lastInstructionLocation();
int8_t instructionLocationByLine(int16_t line);
int variableInAccum(char varName);
int setVariable(char* varName, uint16_t value);
int pushTempVar();
void popTempVar();
int isTempVar(int varName);
void addInstructionv(char* instruction, char varName);
void addInstructiono(char* instruction, int8_t operand);
void addInstructionj(char* instruction, int offset);
void addInstructionEnd();
int setCurrentLine(int16_t line);
void moveAccum(char varName);
void storeAccum(char varName);
void writeInstruction(FILE* fd);

#endif // CONTEXT_H
