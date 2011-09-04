#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

void Term();
void Expression();
void Add();
void Substract();


void Term()
{

    sprintf(tmp,"movl $%c, %%eax", GetNum());
    EmitLn(tmp);
}

void Expression()
{
    Term();
    while (strchr("+-", Look)) {

        EmitLn("movl %eax, %edx");

        switch(Look)
        {
            case '+':
                Add();
                break;
            case '-':
                Substract();
                break;
            default:
                Expected("Addop");
        }
    }
}


void Add()
{
    Match('+');
    Term();
    EmitLn("addl %edx %eax");
}


void Substract()
{
    Match('-');
    Term();
    EmitLn("subl %edx %eax");
    EmitLn("negl %eax");
}


int main()
{
    Init();
    Expression();
    return 0;
}
