#include "mathTree.h"
#include <ctype.h>
#include <malloc.h>
#include "context.h"

MathNode* makeNode()
{
    MathNode* node = malloc(sizeof(*node));
    node->left = NULL;
    node->right = NULL;
    node->operation = NULL;
    node->data = -1;
    return node;
}

int isOperation(char sym)
{
    switch (sym) {
    case '*':
    case '+':
    case '-':
    case '/':
        return 1;
    default:
        return 0;
    }
}

int isOperationBigger(char o1, char o2) {
    if (o1 == '+' || o1 == '-') {
        if (o2 == '*' || o2 == '/')
            return 1;
        else
            return 0;
    } else {
        return 0;
    }
}

static char getAccum(char v1, char v2)
{
    char accum = '\0';
    if (variableInAccum(v1))
        accum = v1;
    if (variableInAccum(v2))
        accum = v2;
    if (accum == '\0') {
        moveAccum(v1);
        accum = v1;
    }
    return accum;
}

static char mulFunc(char lval, char rval)
{
    char accum = getAccum(lval, rval);
    if (accum == lval)
        addInstructionv("MUL", rval);
    else
        addInstructionv("MUL", lval);
    return accum;
}

static char sumFunc(char lval, char rval)
{
    char accum = getAccum(lval, rval);
    if (accum == lval)
        addInstructionv("ADD", rval);
    else
        addInstructionv("ADD", lval);
    return accum;
}

static char subFunc(char lval, char rval)
{
    char accum = getAccum(lval, rval);
    if (accum == lval)
        addInstructionv("SUB", rval);
    else
        addInstructionv("SUB", lval);
    return accum;
}

static char divideFunc(char lval, char rval)
{
    char accum = getAccum(lval, rval);
    if (accum == lval)
        addInstructionv("DIVIDE", rval);
    else
        addInstructionv("DIVIDE", lval);
    return accum;
}

static void setOperation(MathNode* node, char opSym)
{
    switch (opSym) {
    case '*':
        node->operation = mulFunc;
        break;
    case '+':
        node->operation = sumFunc;
        break;
    case '-':
        node->operation = subFunc;
        break;
    case '/':
        node->operation = divideFunc;
        break;
    }
}

struct MathReturn {
    uint16_t idx;
    MathNode* node;
};

struct MathReturn calcS(char* func)
{
    MathNode* node = makeNode();
    uint16_t i;
    for (i = 0; func[i] != ')' && func[i] != '\0'; i++) {
        if (func[i] == ' ')
            continue;
        if (isupper(func[i])) {
            if (node->operation == NULL) {
                node->left = makeNode();
                node->left->data = func[i];
            } else {
                node->right = makeNode();
                node->right->data = func[i];
            }
        } else if (isOperation(func[i])) {
            if (node->operation == NULL) {
                setOperation(node, func[i]);
                node->data = func[i];
            } else {
                if (isOperationBigger(node->data, func[i])) {
                    MathNode* tmp = node->right;
                    struct MathReturn ret = calcS(&func[i]);
                    node->right = ret.node;
                    i += ret.idx;
                    MathNode* left = node->right;
                    while (left->left)
                        left = left->left;
                    left->left = tmp;
                } else {
                    MathNode* tmp = node;
                    node = makeNode();
                    setOperation(node, func[i]);
                    node->data = func[i];
                    node->left = tmp;
                }
            }
        } else if (func[i] == '(') {
            //MathNode* tmp = node->right;
            struct MathReturn ret = calcS(&func[i + 1]);
            if (node->operation == NULL)
                node->left = ret.node;
            else
                node->right = ret.node;
            i += ret.idx + 1;
            //node->right->left = tmp;
        }
    }
    struct MathReturn ret;
    ret.idx = i;
    ret.node = node;
    return ret;
}

MathNode* createTree(char* func)
{
    struct MathReturn ret = calcS(func);
    return ret.node;
}

char eval(MathNode* root)
{
    char lVal, rVal;
    if (root->left->operation == NULL) {
        lVal = root->left->data;
    } else {
        lVal = eval(root->left);
    }
    if (root->right->operation == NULL) {
        rVal = root->right->data;
    } else {
        rVal = eval(root->right);
    }
    return root->operation(lVal, rVal);
}

char evals(char* func)
{
    MathNode* root = createTree(func);
    if (root->operation == NULL)
        return root->left->data;
    return eval(createTree(func));
}
