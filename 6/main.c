#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

#ifdef DEBUG
#define dprint(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define dprint(fmt, ...)
#endif


void Other();
void Block(char *L);
void Condition();
void DoProgram();
void DoIf(char *L);
void DoWhile();
void DoLoop();
void DoRepeat();
void DoFor();
void Expression();
void DoDo();
void DoBreak(char *L);

/* Added in chap6 */
void BoolTerm();
void BoolExpression();
void BoolOr();
void BoolXor();

void Other()
{
    sprintf(tmp, "%c", GetName());
    EmitLn(tmp);
}

void Block(char *L)
{
    while (! strchr("elu", Look)) {
        dprint("Block: get Look = %c\n", Look);
        switch (Look) {
            case 'i':
                DoIf(L);
                break;
            case 'w':
                DoWhile();
                break;
            case 'p':
                DoLoop();
                break;
            case 'r':
                DoRepeat();
                break;
            case 'f':
                DoFor();
                break;
            case 'd':
                DoDo();
                break;
            case 'b':
                DoBreak(L);
            default:
                Other();
                break;
        }
        /* this is for convinent, otherwise newline character will
        cause an error */
        Newline();
    }
}

void Condition()
{
    EmitLn("<codition>");
}

void DoProgram()
{
    Block(NULL);
    if (Look != 'e') {
        Expected("End");
    }
    EmitLn("END");
}

void DoIf(char *L)
{
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, L1);

    Match('i');
    Condition();

    sprintf(tmp, "jz %s", L1);
    EmitLn(tmp);

    Block(L);
    dprint("DoIf: Got Look = %c\n", Look);

    if (Look == 'l') {
        /* match *else* statement */
        Match('l');
        strcpy(L2, NewLabel());

        sprintf(tmp, "jmp %s", L2);
        EmitLn(tmp);

        PostLabel(L1);

        Block(L);
    }

    Match('e');
    PostLabel(L2);
}

void DoWhile()
{
    char L1[MAX_BUF];
    char L2[MAX_BUF];

    Match('w');
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    PostLabel(L1);
    Condition();
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
    Condition();

    sprintf(tmp, "jz %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}

/* I haven't test the actual generated x86 code here, so you're free to
 * inform me if there are bugs. :) */
void DoFor()
{
    char L1[MAX_BUF];
    char L2[MAX_BUF];

    Match('f');
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    char name = GetName();
    Match('=');
    Expression();
    EmitLn("subl %eax, $1");  /* SUBQ #1, D0*/
    sprintf(tmp, "lea %c, %%edx", name);
    EmitLn(tmp);
    EmitLn("movl %eax, (%edx)");
    Expression();
    EmitLn("push %eax"); /* save the execution of expression */
    PostLabel(L1);
    sprintf(tmp, "lea %c, %%edx", name);
    EmitLn(tmp);
    EmitLn("movl (%edx), %eax");
    EmitLn("addl %eax, 1");
    EmitLn("movl %eax, (%edx)");
    EmitLn("cmp (%esp), %eax");
    sprintf(tmp, "jg %s", L2);
    EmitLn(tmp);
    Block(L2);
    Match('e');
    sprintf(tmp, "jmp %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
    EmitLn("pop %eax");
}

void Expression()
{
    EmitLn("<expression>");
}

void DoDo()
{
    Match('d');
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, NewLabel());
    Expression();
    EmitLn("subl %eax, $1");
    EmitLn("movl %eax, %ecx");
    PostLabel(L1);
    EmitLn("pushl %ecx");
    Block(L2);
    EmitLn("popl %ecx");
    sprintf(tmp, "loop %s", L1);
    EmitLn(tmp);
    EmitLn("pushl %ecx");
    PostLabel(L2);
    EmitLn("popl %ecx");
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

void BoolTerm()
{
    if (!IsBoolean(Look)) {
        Expected("Boolean Literal");
    }

    if (GetBoolean()) {
        EmitLn("movl $-1, %eax");
    } else {
        EmitLn("xor %eax, %eax");
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

int main()
{
    Init();
    /*DoProgram();*/
    BoolExpression();
    return 0;
}
