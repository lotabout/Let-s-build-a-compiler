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
void Block();
void Condition();
void DoProgram();
void DoIf();
void DoWhile();
void DoLoop();
void DoRepeat();
void DoFor();
void Expression();
void DoDo();

void Other()
{
    sprintf(tmp, "%c", GetName());
    EmitLn(tmp);
}

void Block()
{
    while (! strchr("elu", Look)) {
        dprint("Block: get Look = %c\n", Look);
        switch (Look) {
            case 'i':
                DoIf();
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
    Block();
    if (Look != 'e') {
        Expected("End");
    }
    EmitLn("END");
}

void DoIf()
{
    char L1[MAX_BUF];
    char L2[MAX_BUF];
    strcpy(L1, NewLabel());
    strcpy(L2, L1);

    Match('i');
    Condition();

    sprintf(tmp, "jz %s", L1);
    EmitLn(tmp);

    Block();
    dprint("DoIf: Got Look = %c\n", Look);

    if (Look == 'l') {
        /* match *else* statement */
        Match('l');
        strcpy(L2, NewLabel());

        sprintf(tmp, "jmp %s", L2);
        EmitLn(tmp);

        PostLabel(L1);

        Block();
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
    Block();
    Match('e');
    sprintf(tmp, "jmp %s", L1);
    EmitLn(tmp);
    PostLabel(L2);
}

void DoLoop()
{
    char L[MAX_BUF];
    Match('p');
    strcpy(L, NewLabel());
    PostLabel(L);
    Block();
    Match('e');
    sprintf(tmp, "jmp %s", L);
    EmitLn(tmp);
}

void DoRepeat()
{
    char L[MAX_BUF];
    Match('r');
    strcpy(L, NewLabel());
    PostLabel(L);
    Block();
    Match('u');
    Condition();

    sprintf(tmp, "jz %s", L);
    EmitLn(tmp);
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
    Block();
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
    char L[MAX_BUF];
    strcpy(L, NewLabel());
    Expression();
    EmitLn("subl %eax, $1");
    EmitLn("movl %eax, %ecx");
    PostLabel(L);
    EmitLn("pushl %ecx");
    Block();
    EmitLn("popl %ecx");
    sprintf(tmp, "loop %s", L);
    EmitLn(tmp);
}

int main()
{
    Init();
    DoProgram();
    return 0;
}
