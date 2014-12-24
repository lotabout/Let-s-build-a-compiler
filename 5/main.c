#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

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
        switch (Look) {
            case 'i':
                DoIf();
                break;
            case 'o':
                Other();
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
    Match('i');
    Condition();
    char *label = NewLabel();
    sprintf(tmp, "jz %s", label);
    EmitLn(tmp);
    Block();
    Match('e');
    PostLabel(label);
}


int main()
{
    Init();
    DoProgram();
    return 0;
}
