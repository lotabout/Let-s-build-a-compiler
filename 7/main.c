/* not that only IF statement is supported for merging lexer and parser in
 * chapter 7.
 */
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

void Other();
void Block();
void DoProgram();
void DoIf();
void DoWhile();
void DoLoop();
void DoRepeat();
void DoFor();
void Expression();
void DoDo();
void DoBreak(char *L);

/* Added in chap6 */
void BoolFactor();
void NotFactor();
void BoolTerm();
void BoolExpression();
void BoolOr();
void BoolXor();
void Relation();
void Equals();
void NotEquals();
void Less();
void Greater();
void Ident();
void Factor();
void SignedFactor();
void Multiply();
void Divide();
void Term();
void Add();
void Subtract();
void Expression();
void Assignment();
void Condition();

void Block()
{
    Scan();
    while (! strchr("el", Token)) {
        dprint("Block: get Look = %c\n", Look);
        switch (Token) {
            case 'i':
                DoIf();
                break;
            case '\n':
                while(Look == '\n') {
                    Fin();
                }
                break;
            default:
                Assignment();
                break;
        }
        Scan();
    }
}

void DoProgram()
{
    Block();
    MatchString("END");
    EmitLn("END");
}

void DoIf()
{
    Condition();
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, L1);

    sprintf(tmp, "jz %s", L1);
    EmitLn(tmp);

    Block();

    if (Token == 'l') {
        /* match *else* statement */
        strcpy(L2, NewLabel());

        sprintf(tmp, "jmp %s", L2);
        EmitLn(tmp);

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

    Match('w');
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    PostLabel(L1);
    BoolExpression();
    sprintf(tmp, "jz %s", L2);
    EmitLn(tmp);
    Block(L2);
    Match('e');
    sprintf(tmp, "jmp %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}

void DoLoop()
{
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    Match('p');
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    PostLabel(L1);
    Block(L2);
    Match('e');
    sprintf(tmp, "jmp %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}

void DoRepeat()
{
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    Match('r');
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    PostLabel(L1);
    Block(L2);
    Match('u');
    BoolExpression();

    sprintf(tmp, "jz %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}


void DoBreak(char *L)
{
    Match('b');
    if (L != NULL) {
        sprintf(tmp, "jmp %s", L);
        EmitLn(tmp);
    } else {
        Abort("No loop to break from");
    }
}

void BoolFactor()
{
    if (IsBoolean(Look)) {
        if (GetBoolean()) {
            EmitLn("movl $-1, %eax");
        } else {
            EmitLn("xor %eax, %eax");
        }
    } else {
        Relation();
    }
}

void Relation()
{
    Expression();
    if (IsRelop(Look)) {
        EmitLn("pushl %eax");
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
        }
    }
    EmitLn("test %eax, %eax");
}

void NotFactor()
{
    if (Look == '!') {
        Match('!');
        BoolFactor();
        EmitLn("xor $-1, %eax");
    } else {
        BoolFactor();
    }
}

void BoolTerm()
{
    NotFactor();
    while(Look == '&') {
        EmitLn("pushl %eax");
        Match('&');
        NotFactor();
        EmitLn("and (%esp), %eax");
        EmitLn("addl $4, %esp");
    }
}

void BoolExpression()
{
    BoolTerm();
    while (IsOrop(Look)) {
        EmitLn("pushl %eax");
        switch (Look) {
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

void BoolOr()
{
    Match('|');
    BoolTerm();
    EmitLn("or (%esp), %eax");
    EmitLn("addl $4, %esp");    /* recover the stack */
}

void BoolXor()
{
    Match('~');
    BoolTerm();
    EmitLn("xor (%esp), %eax");
    EmitLn("addl $4, %esp");    /* recover the stack */
}

void Equals()
{
    Match('=');
    Expression();
    EmitLn("cmp (%esp), %eax");
    /* Note that 80386 has setcc corresponds to 86000's SETCC
     * However, it only takes 8-bit registers */
    EmitLn("sete %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void NotEquals()
{
    Match('#');
    Expression();
    EmitLn("cmp (%esp), %eax");
    EmitLn("setne %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void Less()
{
    Match('<');
    Expression();
    EmitLn("cmp %eax, (%esp)");
    EmitLn("setl %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void Greater()
{
    Match('>');
    Expression();
    EmitLn("cmp %eax, (%esp)");
    EmitLn("setg %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void Ident()
{
    GetName();
    if (Look == '(') {
        Match('(');
        Match(')');
        sprintf(tmp, "call %s", Value);
        EmitLn(tmp);
    } else {
        sprintf(tmp, "movl %s, %%eax", Value);
        EmitLn(tmp);
    }
}

void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (IsAlpha(Look)) {
        Ident();
    } else {
        GetNum();
        sprintf(tmp, "movl $%s, %%eax", Value);
        EmitLn(tmp);
    }
}

void SignedFactor()
{
    bool negative = Look == '-';

    if (Look == '+') {
        GetChar();
        SkipWhite();
    } 

    Factor();

    if (negative) {
        EmitLn("neg %eax");
    }
}

void Multiply()
{
    Match('*');
    Factor();
    EmitLn("imull (%esp), %eax");
    /* push of the stack */
    EmitLn("addl $4, %esp");
}

void Divide()
{
    Match('/');
    Factor();

    /* for a expersion like a/b we have eax=b and %(esp)=a
     * but we need eax=a, and b on the stack
     */
    EmitLn("movl (%esp), %edx");
    EmitLn("addl $4, %esp");

    EmitLn("pushl %eax");

    EmitLn("movl %edx, %eax");

    /* sign extesnion */
    EmitLn("sarl $31, %edx");
    EmitLn("idivl (%esp)");
    EmitLn("addl $4, %esp");

}

void Term1()
{
    while (strchr("*/", Look)) {
        EmitLn("pushl %eax");
        switch(Look)
        {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
            default:
                Expected("Mulop");
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
    SignedFactor();
    Term1();
}

void Add()
{
    Match('+');
    Term();
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
}


void Subtract()
{
    Match('-');
    Term();
    EmitLn("subl (%esp), %eax");
    EmitLn("negl %eax");
    EmitLn("addl $4, %esp");
}

void Expression()
{
    FirstTerm();
    while(IsAddop(Look)) {
        EmitLn("pushl %eax");
        switch (Look) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
            default:
                Expected("Addop");
        }
    }
}

/* This version of 'condition' is dummy */
void Condition()
{
    EmitLn("Condition");
}

void Assignment()
{
    char name[MAX_BUF];
    strcpy(name, Value);
    Match('=');
    Expression();
    sprintf(tmp, "lea %s, %%ebx", name);
    EmitLn(tmp);
    EmitLn("movl %eax, (%ebx)");
}

int main()
{
    Init();
    DoProgram();
    return 0;
}
