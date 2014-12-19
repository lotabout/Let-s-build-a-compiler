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

void Factor()
{
    if(Look == '(') {
        Match('(');
        Expression();
        Match(')');
     } else if(IsAddop(Look)) {
        Match('-');
        sprintf(tmp,"movl $%c, %%eax", GetNum());
        EmitLn(tmp);
        EmitLn("negl %eax");
    } else if (IsAlpha(Look)) {
        sprintf(tmp, "movl %c, %%eax", GetName());
        EmitLn(tmp);
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
    EmitLn(".text");
    EmitLn(".global _start");
    EmitLn("_start:");
    Expression();

    /* return the result */
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
    return 0;
}
