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

void Other()
{
    sprintf(tmp, "%c", GetName());
    EmitLn(tmp);
}

void Block()
{
    while (Look != 'e') {
        dprint("Block: get Look = %c\n", Look);
        switch (Look) {
            case 'i':
                DoIf();
                break;
            default:
                Other();
                break;
        }
        /* this is for convinent, otherwise newline character will
        cause an error */
        Newline();
    }
    Match('e');
}

void Condition()
{
    EmitLn("<codition>");
}

void DoProgram()
{
    Block();
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

int main()
{
    Init();
    DoProgram();
    return 0;
}
