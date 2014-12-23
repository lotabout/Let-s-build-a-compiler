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


/* Not used in Chapter 4 */
void Multiply()
{
    Match('*');
    Factor();
    EmitLn("imull (%esp), %eax");
    /* push of the stack */
    EmitLn("addl $4, %esp");
} 

/* Not used in Chapter 4 */
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
    int factor;
    if (Look == '(') {
        Match('(');
        factor = Expression();
        Match(')');
    } else if (IsAlpha(Look)) {
        factor = Table[GetName() - 'A'];
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


/* Not used in Chapter 4 */
void Add()
{
    Match('+');
    Term();
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
    
}


/* Not used in Chapter 4 */
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
    Table[name - 'A'] = Expression();
}

/* Input Routine
 * We do a little different to the original article.  The syntax of
 * input is "?<variable name><expression>" */
void Input()
{
    Match('?');
    char name = GetName();
    Table[name - 'A'] = Expression();
}

/* Output Routine */
void Output()
{
    Match('!');
    sprintf(tmp, "%d", Table[GetName() - 'A']);
    EmitLn(tmp);
}

int main()
{

    Init();
    do
    {
        switch(Look) {
        case '?':
            Input();
            break;
        case '!':
            Output();
            break;
        default:
            Assignment();
        }

        Newline();
    } while (Look != '.');
    return 0;
}
