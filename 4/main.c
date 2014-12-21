#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

int Term();
int Expression();
void Add();
void Substract();
int Factor();
void Ident();
void Assignment();


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

void Ident()
{
    char name = GetName();
    if (Look == '(') {
        Match('(');
        Match(')');
        sprintf(tmp, "call %c", name);
        EmitLn(tmp);
    } else {
        sprintf(tmp, "movl %c, %%eax", name);
        EmitLn(tmp);
    }
}

int Factor()
{
    /* if(Look == '(') { */
    /*     Match('('); */
    /*     Expression(); */
    /*     Match(')'); */
    /*  } else if(IsAddop(Look)) { */
    /*     Match('-'); */
    /*     sprintf(tmp,"movl $%c, %%eax", GetNum()); */
    /*     EmitLn(tmp); */
    /*     EmitLn("negl %eax"); */
    /* } else if (IsAlpha(Look)) { */
    /*     Ident(); */
    /* } else { */
    /*     sprintf(tmp,"movl $%c, %%eax", GetNum()); */
    /*     EmitLn(tmp); */
    /* } */
    int factor;
    if (Look == '(') {
        Match('(');
        factor = Expression();
        Match(')');
    } else {
        factor = GetNum();
    }

    return factor;
}

int Term()
{
    int value = Factor();
    while (strchr("*/", Look)) {
        switch(Look)
        {
            case '*':
                Match('*');
                value *= Factor();
                break;
            case '/':
                Match('/');
                value /= Factor();
                break;
            default:
                Expected("Mulop");
        }
    }

    return value;
}

int Expression()
{
    int value;
    if(IsAddop(Look))
        value = 0;
    else
        value = Term();

    while (IsAddop(Look)) {
        switch(Look)
        {
            case '+':
                Match('+');
                value += Term();
                break;
            case '-':
                Match('-');
                value -= Term();
                break;
            default:
                Expected("Addop");
        }
    }

    return value;
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

void Assignment()
{
    char name = GetName();
    Match('=');
    Expression();
    sprintf(tmp, "lea %c, %%ebx", name);
    EmitLn(tmp);
    EmitLn("movl %eax, (%ebx)");
}

int main()
{

    Init();
    /* Expression(); */
    /* Assignment(); */
    
    printf("%d\n", Expression());
    if (Look != '\n') {
        Expected("NewLine");
    }

    return 0;
}
