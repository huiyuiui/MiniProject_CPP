#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codeGen.h"
#include "lex.h"

int sbcount = 0;
Symbol table[TBLSIZE];

BTNode* assign_expr();

void initTable(void) {
    strcpy(table[0].name, "x");
    table[0].val = 0;
    strcpy(table[1].name, "y");
    table[1].val = 0;
    strcpy(table[2].name, "z");
    table[2].val = 0;
    sbcount = 3;
}

int getval(char *str) {
    int i = 0;

    for (i = 0; i < sbcount; i++)		//如果輸入字串已被記錄則回傳值
        if (strcmp(str, table[i].name) == 0)
            return table[i].val;
    
    if(i == sbcount) error(UNDEFINED);
    if (sbcount >= TBLSIZE)
        error(RUNOUT);
    
    strcpy(table[sbcount].name, str);	//如果沒有則新建一個字串
    table[sbcount].val = 0;
    sbcount++;
    return 0;
}

int setval(char *str, int val) {
    int i = 0;

    for (i = 0; i < sbcount; i++) {	//如果有則set
        if (strcmp(str, table[i].name) == 0) {
            table[i].val = val;
            return val;
        }
    }

    if (sbcount >= TBLSIZE)
        error(RUNOUT);
    
    strcpy(table[sbcount].name, str);	//沒有則新建再set 
    table[sbcount].val = val;
    sbcount++;
    return val;
}

BTNode *makeNode(TokenSet tok, const char *lexe) {	//把token建成node
    BTNode *node = (BTNode*)malloc(sizeof(BTNode));
    strcpy(node->lexeme, lexe);
    node->data = tok;
    node->val = 0;
    node->idx = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeTree(BTNode *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// factor := INT | ADDSUB INT |
//		   	 ID  | ADDSUB ID  | 
//		   	 ID ASSIGN expr |
//		   	 LPAREN expr RPAREN |
//		   	 ADDSUB LPAREN expr RPAREN
BTNode *factor(void) {
    BTNode *retp = NULL, *left = NULL;

    if (match(INT)) {
        retp = makeNode(INT, getLexeme());
        advance();
    } 
    else if (match(ID)) {
        retp = makeNode(ID, getLexeme());
        advance();
    }
    else if(match(INCDEC)){
        retp = makeNode(INCDEC,getLexeme());
        advance();
        if(match(ID)){
            retp->right = makeNode(ID,getLexeme());
            advance();
        }
        else error(NOTNUMID);
    }
    else if (match(LPAREN)) {
        advance();
        retp = assign_expr();
        if (match(RPAREN))
            advance();
        else
            error(MISPAREN);
    } else {
        error(NOTNUMID);
    }
    return retp;
}

//以下皆已修改
BTNode* unary_expr(){
    BTNode* node = NULL;
    if (match(ADDSUB)) {
        node = makeNode(ADDSUB, getLexeme());  //unary接的是'+-'兩側的一長串
        advance();
        node->left = makeNode(INT,"0");         //運算出來的數字
        node->right = unary_expr();
        return node;
    }
    else return factor();
}

BTNode* muldiv_expr_tail(BTNode* left){
	BTNode* node = NULL;
	if(match(MULDIV)){
		node = makeNode(MULDIV,getLexeme());
		advance();
		node->left = left;
		node->right = unary_expr();
		return muldiv_expr_tail(node);
	} 
	else return left;
}

BTNode* muldiv_expr(){
	BTNode* node = unary_expr();
	return muldiv_expr_tail(node);
}

BTNode* addsub_expr_tail(BTNode* left){
	BTNode* node = NULL;
	if(match(ADDSUB)){
		node = makeNode(ADDSUB,getLexeme());
		advance();
		node->left = left;
		node->right = muldiv_expr();
		return addsub_expr_tail(node);
	}
	else return left;
}

BTNode* addsub_expr(){
	BTNode* node = muldiv_expr();
	return addsub_expr_tail(node);
}

BTNode* and_expr_tail(BTNode* left){
	BTNode* node = NULL;
	if(match(AND)){
		node = makeNode(AND,getLexeme());
		advance();
		node->left = left;
		node->right = addsub_expr();
		return and_expr_tail(node);
	}
	else return left;
}

BTNode* and_expr(){
	BTNode* node = addsub_expr();
	return and_expr_tail(node);
}

BTNode* xor_expr_tail(BTNode* left){
	BTNode* node = NULL;
	if(match(XOR)){
		node = makeNode(XOR,getLexeme());
		advance();
		node->left = left;
		node->right = and_expr();
		return xor_expr_tail(node);
	}
	else return left;
}

BTNode* xor_expr(){
	BTNode* node = and_expr();
	return xor_expr_tail(node);
}

BTNode* or_expr_tail(BTNode* left){
	BTNode* node = NULL;
	if(match(OR)){
		node = makeNode(OR,getLexeme());	//把node建成tree
		advance();
		node->left = left;
		node->right = xor_expr();	//right再去執行下一層，層層遞進
		return or_expr_tail(node);	
	}
	else return left;
}

BTNode* or_expr(){
	BTNode* node = xor_expr();	//一層一層往下找
	return or_expr_tail(node);	
}

BTNode* assign_expr(){
    BTNode* retp = or_expr();   //先執行or_expr
    BTNode* node = NULL;
    if(retp->data == ID && match(ASSIGN)){ //如果抓到的是ID而且右邊為assign則賦值
        node = makeNode(ASSIGN,getLexeme());    
        advance();
        node->left = retp;
        node->right = assign_expr();
        return node;
    }
    else return retp;
} 

// statement := ENDFILE | END | expr END
void statement(void) {
    BTNode *retp = NULL;
    totalIdx = 0;
    if (match(ENDFILE)) {
        printf("MOV r0 [0]\n");
        printf("MOV r1 [4]\n");
        printf("MOV r2 [8]\n");
        printf("EXIT 0\n");
        exit(0);
    } 
    else if (match(END))  advance();	//取得下一個token 
    else {
        retp = assign_expr();
        if (match(END)) {
            evaluateTree(retp);
           //printPrefix(retp),printf("\n");
            printAssemblyCode(retp);  //1代表root傳入
            freeTree(retp);
            advance();
        } else {
            error(SYNTAXERR);
        }
    }
}

void err(ErrorType errorNum) {
    if (PRINTERR) {
        fprintf(stderr, "error: ");
        switch (errorNum) {
            case MISPAREN:
                fprintf(stderr, "mismatched parenthesis\n");
                break;
            case NOTNUMID:
                fprintf(stderr, "number or identifier expected\n");
                break;
            case NOTFOUND:
                fprintf(stderr, "variable not defined\n");
                break;
            case RUNOUT:
                fprintf(stderr, "out of memory\n");
                break;
            case NOTLVAL:
                fprintf(stderr, "lvalue required as an operand\n");
                break;
            case DIVZERO:
                fprintf(stderr, "divide by constant zero\n");
                break;
            case SYNTAXERR:
                fprintf(stderr, "syntax error\n");
                break;
            default:
                fprintf(stderr, "undefined error\n");
                break;
        }
    }
    printf("EXIT 1\n");
    exit(0);
}
