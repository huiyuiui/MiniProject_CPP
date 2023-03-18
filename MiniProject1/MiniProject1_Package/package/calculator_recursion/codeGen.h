#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

extern void checkVar(BTNode* root);

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

extern void printAssemblyCode(BTNode* root);

extern int totalIdx;

#endif // __CODEGEN__
