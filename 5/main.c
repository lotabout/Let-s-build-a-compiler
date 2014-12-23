#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

void Other();
void Block();
void DoProgram();

void Other()
{
    sprintf(tmp, "%c", GetName());
    EmitLn(tmp);
}

void Block()
{
    while (Look != 'e') {
        Other();

        /* this is for convinent, otherwise newline character will
        cause an error */
        Newline(); 
    }
}

void DoProgram()
{
    Block();
    if (Look != 'e') {
        Expected("End");
    }
    EmitLn("END");
}

int main()
{
    Init();
    DoProgram();
    return 0;
}
