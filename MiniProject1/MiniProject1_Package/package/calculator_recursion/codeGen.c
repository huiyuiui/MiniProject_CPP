#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"
#include "parser.h"

int idx = 0;
int varFlag;

void checkVar(BTNode* root){
    if(root == NULL) return;
    if(root->data == ID){
        varFlag = 1;
        return;
    }
    if(varFlag == 0) checkVar(root->left);
    if(varFlag == 0) checkVar(root->right);   
}

int evaluateTree(BTNode *root) {
    int retval = 0, lv = 0, rv = 0;

    if (root != NULL) {
        switch (root->data) {
            case ID:
                retval = getval(root->lexeme);
                break;
            case INT:
                retval = atoi(root->lexeme);
                break;
            case ASSIGN:
                rv = evaluateTree(root->right);
                retval = setval(root->left->lexeme, rv);
                break;
            case ADDSUB:
            case MULDIV:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                if (strcmp(root->lexeme, "+") == 0) {
                    retval = lv + rv;
                } else if (strcmp(root->lexeme, "-") == 0) {
                    retval = lv - rv;
                } else if (strcmp(root->lexeme, "*") == 0) {
                    retval = lv * rv;
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if (rv == 0){
                        varFlag = 0;
                        checkVar(root->right);
                        if(varFlag == 0) error(DIVZERO);
                    }
                }
                break;
            case INCDEC:
                rv = evaluateTree(root->right);
                if(strcmp(root->lexeme,"++") == 0) 
                    retval = setval(root->right->lexeme,rv+1);
                else if(strcmp(root->lexeme,"--") == 0) 
                    retval = setval(root->right->lexeme,rv-1);
                break;
            case OR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                retval = lv | rv;
                break;
            case XOR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                retval = lv ^ rv;
                break;
            case AND:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                retval = lv & rv;
                break;
            default:
                retval = 0;
        }
    }
    return retval;
}

void printPrefix(BTNode *root) {
    if (root != NULL) {
        printf("%s ", root->lexeme);
        printPrefix(root->left);
        printPrefix(root->right);
    }
}

int totalIdx;
void printAssemblyCode(BTNode* root){  
    if(root == NULL) return;
    if(root->data == ASSIGN) printAssemblyCode(root->right);    //如果為assign則先做右邊
    else{
        printAssemblyCode(root->left);
        printAssemblyCode(root->right);
    }
    if(root->idx == 0 && (root->data == ID || root->data == INT)){
        root->idx = totalIdx;
        totalIdx++; //新的registers
    }
    else if(root->data == INCDEC) root->idx = totalIdx; //新的registers儲存
    else if(root->data == OR || root->data == XOR || root->data == AND ||
            root->data == ADDSUB || root->data == MULDIV){
                root->idx = root->left->idx;    //都只抓左邊的idx
                totalIdx--;     //抓完之後儲存到左邊，所以右邊的可以不見
    }
    int memory;
    switch (root->data){
    case ID:
        for(int i=0;i<sbcount;i++){
            if(strcmp(table[i].name,root->lexeme) == 0){
                memory = i * 4;
                break;
            }
        }
        printf("MOV r%d [%d]\n",root->idx,memory);
        break;
    case INT:
        printf("MOV r%d %d\n",root->idx,atoi(root->lexeme));
        break;
    case ASSIGN:
        for(int i=0;i<sbcount;i++){
            if(strcmp(table[i].name,root->left->lexeme) == 0){
                memory = i * 4;
                break;
            }
        }
        printf("MOV [%d] r%d\n",memory,root->right->idx);
        root->idx = root->right->idx;   //此處抓右邊因為把右邊的assign給左邊
        break;
    case OR:
        printf("OR r%d r%d\n",root->left->idx,root->right->idx);
        break;
    case XOR:
        printf("XOR r%d r%d\n",root->left->idx,root->right->idx);
        break;
    case AND:
        printf("AND r%d r%d\n",root->left->idx,root->right->idx);
        break;
    case ADDSUB:
        if(strcmp(root->lexeme,"+") == 0){
            printf("ADD r%d r%d\n",root->left->idx,root->right->idx);
        }
        else if(strcmp(root->lexeme,"-") == 0){
            printf("SUB r%d r%d\n",root->left->idx,root->right->idx);
        }
        break;
    case MULDIV:
        if(strcmp(root->lexeme,"*") == 0){
            printf("MUL r%d r%d\n",root->left->idx,root->right->idx);
        }
        else if(strcmp(root->lexeme,"/") == 0){
            printf("DIV r%d r%d\n",root->left->idx,root->right->idx);
        }
        break;
    case INCDEC:
        for(int i=0;i<sbcount;i++){
            if(strcmp(table[i].name,root->right->lexeme) == 0){
                memory = i*4;
                break;
            }
        }
        printf("MOV r%d 1\n",root->idx);  //把1存到本身的idx
        if(strcmp(root->lexeme,"++") == 0){
            printf("ADD r%d r%d\n",root->right->idx,root->idx);
        }
        else if(strcmp(root->lexeme,"--") == 0){
            printf("SUB r%d r%d\n",root->right->idx,root->idx);
        }
        printf("MOV [%d] r%d\n",memory,root->right->idx);
    default:
        break;
    }
}
