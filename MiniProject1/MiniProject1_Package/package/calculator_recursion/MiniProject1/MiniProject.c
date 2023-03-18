#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 256

// Token types
typedef enum {
    UNKNOWN, END, ENDFILE, 
    INT, ID,
    ADDSUB, MULDIV, INCDEC,
    ASSIGN, 
    LPAREN, RPAREN, 
	AND, OR, XOR, 
} TokenSet;

// Test if a token matches the current token 
int match(TokenSet token);

// Get the next token
void advance(void);

// Get the lexeme of the current token
char *getLexeme(void);

#define TBLSIZE 64

// Set PRINTERR to 1 to print error message while calling error()
// Make sure you set PRINTERR to 0 before you submit your code
#define PRINTERR 1

// Call this macro to print error message and exit the program
// This will also print where you called it in your program
#define error(errorNum) { \
    if (PRINTERR) \
        fprintf(stderr, "error() called at %s:%d: ", __FILE__, __LINE__); \
    err(errorNum); \
}

// Error types
typedef enum {
    UNDEFINED, MISPAREN, NOTNUMID, NOTFOUND, RUNOUT, NOTLVAL, DIVZERO, SYNTAXERR
} ErrorType;

// Structure of the symbol table
typedef struct {
    int val;
    char name[MAXLEN];
} Symbol;

// Structure of a tree node
typedef struct _Node {
    TokenSet data;
    int val;
    int idx;
    int flag; //0代表在左邊 1代表在右邊
    char lexeme[MAXLEN];
    struct _Node *left; 
    struct _Node *right;
} BTNode;

// The symbol table
Symbol table[TBLSIZE];

// Initialize the symbol table with builtin variables
void initTable(void);

// Get the value of a variable
int getval(char *str);

// Set the value of a variable
 int setval(char *str, int val);

// Make a new node according to token type and lexeme
 BTNode *makeNode(TokenSet tok, const char *lexe);

// Free the syntax tree
 void freeTree(BTNode *root);

BTNode* assign_expr(void);
BTNode* or_expr(void);
BTNode* or_expr_tail(BTNode* left);
BTNode* xor_expr(void);
BTNode* xor_expr_tail(BTNode* left);
BTNode* and_expr(void);
BTNode* and_expr_tail(BTNode* left);
BTNode* addsub_expr(void);
BTNode* addsub_expr_tail(BTNode* left);
BTNode* muldiv_expr(void);
BTNode* muldiv_expr_tail(BTNode* left);
BTNode* unary_expr(void);
BTNode *factor(void);

void statement(void);

// Print error message and exit the program
void err(ErrorType errorNum);

int sbcount;
int varFlag;


void checkVar(BTNode* root);

// Evaluate the syntax tree
int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
void printPrefix(BTNode *root);

void printAssemblyCode(BTNode* root);

int totalIdx;

static TokenSet getToken(void);
static TokenSet curToken = UNKNOWN;
static char lexeme[MAXLEN]; //運算元，ID或是數字

TokenSet getToken(void)
{
    int i = 0;
    char c = '\0';

    while ((c = fgetc(stdin)) == ' ' || c == '\t');

    if (isdigit(c)) {		//取數字
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while (isdigit(c) && i < MAXLEN) {	
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        } 
        ungetc(c, stdin);	//取消空格輸入
        lexeme[i] = '\0';
        return INT;
    } //已修改
	else if (c == '+' || c == '-') {
        lexeme[0] = c;
        c = fgetc(stdin);
        if((lexeme[0] == '+' && c == '+') || (lexeme[0] == '-' && c == '-')){
        	lexeme[1] = c;
        	lexeme[2] = '\0';
        	return INCDEC;
		}
        ungetc(c,stdin);
		lexeme[1] = '\0';
		return ADDSUB;	
    }
	else if (c == '*' || c == '/') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return MULDIV;
    } else if (c == '\n') {
        lexeme[0] = '\0';
        return END;
    } else if (c == '=') {
        strcpy(lexeme, "=");
        return ASSIGN;
    } else if (c == '(') {
        strcpy(lexeme, "(");
        return LPAREN;
    } else if (c == ')') {
        strcpy(lexeme, ")");
        return RPAREN;
    } //已修改
	else if(c == '&'){
		lexeme[0] = c;
		lexeme[1] = '\0';
		return AND;
	} 
	else if(c == '|'){
		lexeme[0] = c;
		lexeme[1] = '\0';
		return OR;
	} 
	else if(c == '^'){
		lexeme[0] = c;
		lexeme[1] = '\0';
		return XOR;
	} 
	else if (isalpha(c) || c == '_') {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while((isalnum(c) || c == '_') && i < MAXLEN){
        	lexeme[i] = c;
        	++i;
        	c = fgetc(stdin);
		}
		ungetc(c,stdin);
		lexeme[i] = '\0';
        return ID;
    } 
	else if (c == EOF) {
        return ENDFILE;
    } 
	else {
        return UNKNOWN;
    }
}

void advance(void) {
    curToken = getToken();  //取得現在的token
}

int match(TokenSet token) {
    if (curToken == UNKNOWN)    //如果token無法辨識則取下一個token
        advance();
    return token == curToken;   //如果傳入token與curtoken相符合則回傳1
}

char *getLexeme(void) {
    return lexeme;
}

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
            //printPrefix(retp);
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
        root->idx = root->right->idx;
        break;
    default:
        break;
    }
}


int main() {
    initTable();
    while (1) {
        statement();
    }
    return 0;
}
