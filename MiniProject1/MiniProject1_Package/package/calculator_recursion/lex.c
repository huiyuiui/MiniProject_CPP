#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

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
