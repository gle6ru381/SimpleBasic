#ifndef MATHTREE_H
#define MATHTREE_H
#include <inttypes.h>

typedef char (*MathFunc)(char,char);

typedef struct _MathNode {
    char data;
    struct _MathNode* left;
    struct _MathNode* right;
    MathFunc operation;
} MathNode;

MathNode* createTree(char* func);
char eval(MathNode* root);
char evals(char* func);

#endif // MATHTREE_H
