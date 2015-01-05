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
void Alloc(char);
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

void Prog()
{
    Match('p');     /* handles program header part */
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
    Match('b');
    Prolog();
    Block();
    Match('e');
    Epilog();
}

void TopDecls()
{
    while(Look != 'b') {
        switch(Look) {
            case 'v':
                Decl();
                break;
            default:
                sprintf(tmp, "Unrecognized Keyword '%c'", Look);
                Abort(tmp);
                break;
        }
    }
}

void Decl()
{
    EmitLn(".section .data"); /* in case that the variable and function
                                 declarations are mixed */
    Match('v');
    Alloc(GetName());
    while(Look == ',') {
        GetChar();
        Alloc(GetName());
    }
}

void Alloc(char name)
{
    if (InTable(name)) {
        sprintf(tmp, "Duplicate Variable Name: %c", name);
        Abort(tmp);
    }
    ST[name - 'A'] = 'v';
    sprintf(tmp, "%c: .int ", name);
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

void Block()
{
    while(strchr("e", Look) == NULL) {
        Assignment();
    }
}

void Assignment()
{
    char name = GetName();
    Match('=');
    Expression();
    Store(name);
}

void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (IsAlpha(Look)) {
        LoadVar(GetName());
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
    }
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
