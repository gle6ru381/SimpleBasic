#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include "context.h"
#include "instruction.h"
#include <string.h>

int readLine(FILE* fd, char* buff)
{
    char sym;
    int i = 0;
    while(1) {
        sym = fgetc(fd);
        if (sym == EOF)
            break;
        if (sym == '\n')
            break;
        buff[i++] = sym;
    }
    buff[i] = '\0';
    return i;
}

int main(int argc, char** argv)
{
    FILE* in, *out;
    initContext();
    if (argc != 3) {
        in = fopen("test.sb", "r");
        out = fopen("test.sa", "w");
    } else {
    in = fopen(argv[1], "r");
    if (in == NULL) {
        fprintf(stderr, "Неудалось открый файл\n");
        return -2;
    }
    out = fopen(argv[2], "w");
    if (out == NULL) {
        fprintf(stderr, "Неудалось создать файл\n");
        return -3;
    }
    }
    char* buff = malloc(sizeof(char) * 500);
    while (!feof(in)) {
        int strLen = readLine(in, buff);
        if (strLen == 0)
            continue;
        int i = 3, j = 0;
        char instr[10];
        int8_t flag = 0;
        for (; i < strLen; i++) {
            if (buff[i] == ' ') {
                if (flag)
                    break;
                else
                    continue;
            }
            if (isupper(buff[i])) {
                flag = 1;
                instr[j++] = buff[i];
            } else {
                fprintf(stderr, "Некорректная инструкция\n");
                return -4;
            }
            instr[j] = '\0';
            if (!strcmp("LET", instr) || !strcmp("IF", instr)) {
                findLiterals(&buff[i + j]);
            }
        }
    }
    fseek(in, 0, SEEK_SET);
    while (!feof(in)) {
        int strLen = readLine(in, buff);
        if (strLen == 0)
            continue;
        char lineNumber[3] = {buff[0], buff[1], '\0'};
        int16_t line = atoi(lineNumber);
        setCurrentLine(line);
        char instr[10];
        uint16_t j = 0, i = 3;
        int8_t flag = 0;
        for (; i < strLen; i++) {
            if (buff[i] == ' ') {
                if (flag)
                    break;
                else
                    continue;
            }
            if (isupper(buff[i])) {
                flag = 1;
                instr[j++] = buff[i];
            } else {
                fprintf(stderr, "Некорректная инструкция на строке %d\n", (int)line);
                return -4;
            }
        }
        instr[j] = '\0';
        int retVal = invokeInstr(instr, buff + i + 1);
        if (retVal) {
            fprintf(stderr, "Некорректный операнд на строке %d\n", (int)line);
            return -5;
        }
    }
    writeInstruction(out);
    return 0;
}
