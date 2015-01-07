#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cradle.h"

#ifdef DEBUG
#define dprint(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define dprint(fmt, ...)
#endif


void TopDecls();
void Allocate(char *name, char *value);
void Alloc();
void Block();
void Assignment();

void Factor();
void Expression();
void Subtract();
void Term();
void Divide();
void Multiply();
void FirstFactor();
void Add();
void Equals();
void NotEqual();
void Less();
void LessOrEqual();
void Greater();
void Relation();
void NotFactor();
void BoolTerm();
void BoolOr();
void BoolXor();
void BoolExpression();
void DoIf();
void DoWhile();
void CompareExpression();
void NextExpression();

void Semi();

void Header()
{
    EmitLn(".global _start");
}

void Prolog()
{
    EmitLn(".section .text");
    EmitLn("_start:");
}

void Epilog()
{
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
}

void TopDecls()
{
    Scan();
    while(Token == 'v') {
        EmitLn(".section .data"); /* in case that the variable and function
                                     declarations are mixed */
        Alloc();
        while(Token == ',') {
            Alloc();
        }
        Semi();
    }
}

/* Allocate Storage for a static variable */
void Allocate(char *name, char *value)
{
    sprintf(tmp, "%s: .int %s", name, value);
    EmitLn(tmp);
}

void Alloc()
{
    char name[MAX_BUF];
    Next();
    if (Token != 'x') {
        Expected("Variable Name");
    }
    CheckDup(Value);

    sprintf(name, Value);
    AddEntry(name, 'v');
    Next();
    if (Token == '=') {
        Next();
        if (Token != '#') {
            Expected("Integer");
        }
        Allocate(name, Value);
        Next();
    } else {
        Allocate(name, "0");
    }
}

/* Parse and Translate a Block of Statements 
 * <block> ::= ( <statement> )*
 * <statement> ::= <if> | <while> | <assignment>
 * */
void Block()
{
    Scan();
    while(strchr("el", Token) == NULL) {
        switch (Token) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'x':
                Assignment();
                break;
            default:
                break;
        }
        Semi();
        Scan();
    }
}

void Assignment()
{
    char name[MAX_BUF];
    sprintf(name, Value);
    Next();
    MatchString("=");
    BoolExpression();
    Store(name);
}

void Factor()
{
    if (Token == '(') {
        Next();
        BoolExpression();
        MatchString(")");
    } else {
        if (Token == 'x') {
            LoadVar(Value);
        } else if (Token == '#') {
            LoadConst(Value);
        } else {
            Expected("Math Factor");
        }
        Next();
    }
}


void Multiply()
{
    Next();
    Factor();
    PopMul();
}

void Divide()
{
    Next();
    Factor();
    PopDiv();
}

void Term()
{
    Factor();
    while(IsMulop(Token)) {
        Push();
        switch(Token) {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
            default:
                break;
        }
    }
}

void Add()
{
    Next();
    Term();
    PopAdd();
}

void Subtract()
{
    Next();
    Term();
    PopSub();
}

void Expression()
{
    if (IsAddop(Token)) {
        Clear();
    } else {
        Term();
    }

    while(IsAddop(Token)) {
        Push();
        switch(Token) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
            default:
                break;
        }
    }
}

/* Get another expression and compare */
void CompareExpression()
{
    Expression();
    PopCompare();
}

/* Get the next expression and compare */
void NextExpression()
{
    Next();
    CompareExpression();
}

/* Recognize and Translate a Relational "Equals" */
void Equals()
{
    NextExpression();
    SetEqual();
}

/* Recognize and Translate a Relational "Not Equals" */
void NotEqual()
{
    NextExpression();
    SetNEqual();
}

/* Recognize and Translate a Relational "Less Than" */
void Less()
{
    Next();
    switch(Token) {
        case '=':
            LessOrEqual();
            break;
        case '>':
            NotEqual();
            break;
        default:
            CompareExpression();
            SetLess();
            break;
    }
}

/* Recognize and Translate a Relational "Less or Equal" */
void LessOrEqual()
{
    NextExpression();
    SetLessOrEqual();
}

/* Recognize and Translate a Relational "Greater Than" */
void Greater()
{
    Next();
    if (Token == '=') {
        NextExpression();
        SetGreaterOrEqual();
    } else {
        CompareExpression();
        SetGreater();
    }
}

/* Parse and Translate a Relation */
void Relation()
{
    Expression();
    if (IsRelop(Token)) {
        Push();
        switch (Token) {
            case '=':
                Equals();
                break;
            case '<':
                Less();
                break;
            case '>':
                Greater();
                break;
            default:
                break;
        }
    }
}

/* Parse and Translate a Boolean Factor with Leading NOT */
void NotFactor()
{
    if (Token == '!') {
        Next();
        Relation();
        NotIt();
    } else {
        Relation();
    }
}

/* Parse and Translate a Boolean Term 
 * <bool_term> ::= <not_factor> ( and_op <not_factor )*
 * */
void BoolTerm()
{
    NotFactor();
    while(Token == '&') {
        Push();
        Next();
        NotFactor();
        PopAnd();
    }
}

/* Recognize and Translate a Boolean OR */
void BoolOr()
{
    Next();
    BoolTerm();
    PopOr();
}

/* Recognize and Translate a Boolean XOR */
void BoolXor()
{
    Next();
    BoolTerm();
    PopXor();
}

/* Parse and Translate a Boolean Expression 
 * <bool_expression> ::= <bool_term> ( or_op <bool_term> )* */
void BoolExpression()
{
    BoolTerm();
    while(IsOrOp(Token)) {
        Push();
        switch(Look) {
            case '|':
                BoolOr();
                break;
            case '~':
                BoolXor();
                break;
            default:
                break;
        }
    }
}

/* Recognize and Translate an IF construct */
void DoIf()
{
    Next();
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    sprintf(L1, NewLabel());
    sprintf(L2, L1);
    BoolExpression();
    BranchFalse(L1);
    Block();
    if (Token == 'l') {
        Next();
        sprintf(L2, NewLabel());
        Branch(L2);
        PostLabel(L1);
        Block();
    }
    PostLabel(L2);
    MatchString("ENDIF");
}

void DoWhile()
{
    Next();
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    sprintf(L1, NewLabel());
    sprintf(L2, NewLabel());
    PostLabel(L1);
    BoolExpression();
    BranchFalse(L2);
    Block();
    MatchString("ENDWHILE");
    Branch(L1);
    PostLabel(L2);
}

void Semi()
{
    /* make a semicolon optional */
    if (Token == ';') {
        Next();
    }
}

int main()
{
    Init();
    MatchString("PROGRAM");
    Semi();
    Header();
    TopDecls();
    MatchString("BEGIN");
    Prolog();
    Block();
    MatchString("END");
    Epilog();

    return 0;
}
