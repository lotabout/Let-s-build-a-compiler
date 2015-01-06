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


void Prog();
void Prolog();
void Epilog();
void Header();
void Main();
void Decl();
void TopDecls();
void Alloc(char *);
void Block();
void Assignment();

void Factor();
void NegFactor();
void Expression();
void Subtract();
void FirstTerm();
void Term();
void Term1();
void Divide();
void Multiply();
void FirstFactor();
void Add();
void Equals();
void NotEquals();
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

void Prog()
{
    MatchString("PROGRAM");
    Header();
    TopDecls();
    Main();
    Match('.');
}

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

void Main()
{
    MatchString("BEGIN");
    Prolog();
    Block();
    MatchString("END");
    Epilog();
}

void TopDecls()
{
    NewLine();
    Scan();
    while(Token != 'b') {
        switch(Token) {
            case 'v':
                Decl();
                break;
            default:
                sprintf(tmp, "Unrecognized Keyword '%c'", Look);
                Abort(tmp);
                break;
        }
        Scan();
        NewLine();
    }
}

void Decl()
{
    NewLine();
    EmitLn(".section .data"); /* in case that the variable and function
                                 declarations are mixed */
    GetName();
    Alloc(Value);
    while(Look == ',') {
        Match(',');
        GetName();
        Alloc(Value);
        NewLine();
    }
}

void Alloc(char *name)
{
    if (InTable(name)) {
        sprintf(tmp, "Duplicate Variable Name: %s", name);
        Abort(tmp);
    }
    AddEntry(name, 'v');
    sprintf(tmp, "%s: .int ", name);
    Emit(tmp);
    if (Look == '=') {
        Match('=');
        if (Look == '-') {
            Emit("-");
            Match('-');
        } else {
            Emit("");
        }
        printf("%d\n", GetNum());
    } else {
        EmitLn("0");
    }
}

/* Parse and Translate a Block of Statements 
 * <block> ::= ( <statement> )*
 * <statement> ::= <if> | <while> | <assignment>
 * */
void Block()
{
    Scan();
    NewLine();
    while(strchr("el", Token) == NULL) {
        switch (Token) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            default:
                Assignment();
                break;
        }
        Scan();
        NewLine();
    }
}

void Assignment()
{
    char name[MAX_BUF];
    sprintf(name, Value);
    Match('=');
    BoolExpression();
    Store(name);
}

void Factor()
{
    if (Look == '(') {
        Match('(');
        BoolExpression();
        Match(')');
    } else if (IsAlpha(Look)) {
        GetName();
        LoadVar(Value);
    } else {
        LoadConst(GetNum());
    }
}

void NegFactor()
{
    Match('-');
    if (IsDigit(Look)) {
        LoadConst(-GetNum());
    } else {
        Factor();
        Negate();
    }
}

/* Parse and Translate a Leading Factor */
void FirstFactor()
{
    switch (Look) {
        case '+':
            Match('+');
            Factor();
            break;
        case '-':
            NegFactor();
            break;
        default:
            Factor();
    }
}

void Multiply()
{
    Match('*');
    Factor();
    PopMul();
}

void Divide()
{
    Match('/');
    Factor();
    PopDiv();
}

void Term1()
{
    NewLine();
    while(IsMulop(Look)) {
        Push();
        switch(Look) {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
            default:
                break;
        }
        NewLine();
    }
}

void Term()
{
    Factor();
    Term1();
}

void FirstTerm()
{
    FirstFactor();
    Term1();
}

void Add()
{
    Match('+');
    Term();
    PopAdd();
}

void Subtract()
{
    Match('-');
    Term();
    PopSub();
}

void Expression()
{
    NewLine();
    FirstTerm();
    while(IsAddop(Look)) {
        Push();
        switch(Look) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
            default:
                break;
        }
        NewLine();
    }
}

/* Recognize and Translate a Relational "Equals" */
void Equals()
{
    Match('=');
    Expression();
    PopCompare();
    SetEqual();
}

/* Recognize and Translate a Relational "Not Equals" */
void NotEquals()
{
    Match('>');
    Expression();
    PopCompare();
    SetNEqual();
}

/* Recognize and Translate a Relational "Less Than" */
void Less()
{
    Match('<');
    switch(Look) {
        case '=':
            LessOrEqual();
            break;
        case '>':
            NotEquals();
            break;
        default:
            Expression();
            PopCompare();
            SetLess();
            break;
    }
}

/* Recognize and Translate a Relational "Less or Equal" */
void LessOrEqual()
{
    Match('=');
    Expression();
    PopCompare();
    SetLessOrEqual();
}

/* Recognize and Translate a Relational "Greater Than" */
void Greater()
{
    Match('>');
    if (Look == '=') {
        Match('=');
        Expression();
        PopCompare();
        SetGreaterOrEqual();
    } else {
        Expression();
        PopCompare();
        SetGreater();
    }
}

/* Parse and Translate a Relation */
void Relation()
{
    Expression();
    if (IsRelop(Look)) {
        Push();
        switch (Look) {
            case '=':
                Equals();
                break;
            case '#':
                NotEquals();
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
    if (Look == '!') {
        Match('!');
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
    NewLine();
    NotFactor();
    while(Look == '&') {
        Push();
        Match('&');
        NotFactor();
        PopAnd();
        NewLine();
    }
}

/* Recognize and Translate a Boolean OR */
void BoolOr()
{
    Match('|');
    BoolTerm();
    PopOr();
}

/* Recognize and Translate a Boolean XOR */
void BoolXor()
{
    Match('~');
    BoolTerm();
    PopXor();
}

/* Parse and Translate a Boolean Expression 
 * <bool_expression> ::= <bool_term> ( or_op <bool_term> )* */
void BoolExpression()
{
    NewLine();
    BoolTerm();
    while(IsOrOp(Look)) {
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
        NewLine();
    }
}

/* Recognize and Translate an IF construct */
void DoIf()
{
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    sprintf(L1, NewLabel());
    sprintf(L2, L1);
    BoolExpression();
    BranchFalse(L1);
    Block();
    if (Token == 'l') {
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


int main()
{
    Init();
    Prog();
    if (Look != '\n') {
        Abort("Unexpected data after '.'");
    }
    return 0;
}
