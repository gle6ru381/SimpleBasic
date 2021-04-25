#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "context.h"
#include "mystring.h"

int genInputIns(char* instr);
int genPrintIns(char* instr);
int genLetIns(char* instr);
int genIfIns(char* instr);
int genGotoIns(char* instr);

int invokeInstr(char* instr, char* operand);

#endif // INSTRUCTION_H
