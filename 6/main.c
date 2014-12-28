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
void BoolFactor();
void NotFactor();
void BoolTerm();
void BoolExpression();
void BoolOr();
void BoolXor();
void Relation();
void Equals();
void NotEquals();
void Less();
void Greater();
void Ident();
void Factor();
void SignedFactor();
void Multiply();
void Divide();
void Term();
void Add();
void Subtract();
void Expression();

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
    BoolExpression();

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
    BoolExpression();
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
    BoolExpression();

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

void BoolFactor()
{
    if (IsBoolean(Look)) {
        if (GetBoolean()) {
            EmitLn("movl $-1, %eax");
        } else {
            EmitLn("xor %eax, %eax");
        }
    } else {
        Relation();
    }
}

void Relation()
{
    Expression();
    if (IsRelop(Look)) {
        EmitLn("pushl %eax");
        switch (Look) {
            case '=':
                Equals();
                break;
            case '#':
                NotEquals();
                break;
            case '<':
                Less();
                break;
            case '>':
                Greater();
                break;
        }
    }
    EmitLn("test %eax, %eax");
}

void NotFactor()
{
    if (Look == '!') {
        Match('!');
        BoolFactor();
        EmitLn("xor $-1, %eax");
    } else {
        BoolFactor();
    }
}

void BoolTerm()
{
    NotFactor();
    while(Look == '&') {
        EmitLn("pushl %eax");
        Match('&');
        NotFactor();
        EmitLn("and (%esp), %eax");
        EmitLn("addl $4, %esp");
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

void Equals()
{
    Match('=');
    Expression();
    EmitLn("cmp (%esp), %eax");
    /* Note that 80386 has setcc corresponds to 86000's SETCC 
     * However, it only takes 8-bit registers */
    EmitLn("sete %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void NotEquals()
{
    Match('#');
    Expression();
    EmitLn("cmp (%esp), %eax");
    EmitLn("setne %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void Less()
{
    Match('<');
    Expression();
    EmitLn("cmp %eax, (%esp)");
    EmitLn("setl %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void Greater()
{
    Match('>');
    Expression();
    EmitLn("cmp %eax, (%esp)");
    EmitLn("setg %al");
    EmitLn("addl $4, %esp");     /* recover the stack */
}

void Ident()
{
    char c = GetName();
    if (Look == '(') {
        Match('(');
        Match(')');
        sprintf(tmp, "call %c", c);
        EmitLn(tmp);
    } else {
        sprintf(tmp, "movl %c, %%eax", c);
        EmitLn(tmp);
    }
}

void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (IsAlpha(Look)) {
        Ident();
    } else {
        sprintf(tmp, "movl $%d, %%eax", GetNum());
        EmitLn(tmp);
    }
}

void SignedFactor()
{
    if (Look == '+') {
        GetChar();
        Factor();
    } else if (Look == '-') {
        GetChar();
        if (IsDigit(Look)) {
            sprintf(tmp, "movl $-%d, %%eax", GetNum());
            EmitLn(tmp);
        } else {
            Factor();
            EmitLn("neg %eax");
        }
    } else {
        Factor();
    }
}

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

void Term()
{
    SignedFactor();
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

void Add()
{
    Match('+');
    Term();
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
}


void Subtract()
{
    Match('-');
    Term();
    EmitLn("subl (%esp), %eax");
    EmitLn("negl %eax");
    EmitLn("addl $4, %esp");
}

void Expression()
{
    Term();
    while(IsAddop(Look)) {
        EmitLn("pushl %eax");
        switch (Look) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
            default:
                Expected("Addop");
        }
    }
}

int main()
{
    Init();
    DoProgram();
    return 0;
}
