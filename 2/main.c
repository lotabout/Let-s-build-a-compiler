#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

void Term();
void Expression();
void Add();
void Substract();
void Factor();


void Multiply()
{
    Match('*');
    Factor();
    EmitLn("imull (%esp), %eax");
    EmitLn("addl $4, %esp");
}

void Divide()
{
    Match('/');
    Factor();
    EmitLn("div (%esp) %eax");
    EmitLn("addl $4, %esp");
}

void Factor()
{

    if(Look == '(') {

        Match('(');
        Expression();
        Match(')');

    } else {

        sprintf(tmp,"movl $%c, %%eax", GetNum());
        EmitLn(tmp);
    }
}

void Term()
{
    Factor();
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

void Expression()
{
    if(IsAddop(Look))
        EmitLn("xor %eax, %eax");
    else
        Term();

    while (strchr("+-", Look)) {

        EmitLn("pushl %eax");

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
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
    
}


void Substract()
{
    Match('-');
    Term();
    EmitLn("subl (%esp), %eax");
    EmitLn("negl %eax");
    EmitLn("addl $4, %esp");
}


int main()
{
    Init();
    Expression();
    return 0;
}
