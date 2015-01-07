#include <stdio.h>
#include <string.h>

#include "cradle.h"

void Expression();
void AssignOrProc();
void Assignment(char name);
void DoBlock();
void BeginBlock();
void Alloc(char name);
void Decl(void);
void TopDecls(void);
void DoProc(void);
void DoMain(void);
void Return();
void CallProc(char name);

void Header();
void Prolog();
void Epilog();


/* parse and tranlate an expression
 * vestigial version */
void Expression()
{
    LoadVar(GetName());
}

/* decide if a statement is an assignment or procedure call */
void AssignOrProc()
{
    char name = GetName();
    char tmp_buf[MAX_BUF];
    switch (TypeOf(name)) {
        case ' ':
            Undefined(name);
            break;
        case 'v':
            Assignment(name);
            break;
        case 'p':
            CallProc(name);
            break;
        default:
            sprintf(tmp_buf, "Identifier %c cannot be used here", name);
            Abort(tmp_buf);
            break;
    }
}

/* parse and tranlate an assignment statement */
void Assignment(char name)
{
    Match('=');
    Expression();
    StoreVar(name);
}

/* parse and translate a block of statement */
void DoBlock()
{
    while(strchr("e", Look) == NULL) {
        AssignOrProc();
        Fin();
    }
}

/* call a procedure */
void CallProc(char name)
{
    char tmp_buf[MAX_BUF];
    sprintf(tmp_buf, "call %c", name);
    EmitLn(tmp_buf);
}

/* parse and translate a Begin-Block */
void BeginBlock()
{
    Match('b');
    Fin();
    DoBlock();
    Match('e');
    Fin();
}

/* allocate storage for a variable */
void Alloc(char name)
{
    if (InTable(name)) {
        Duplicate(name);
    }
    ST[name-'A'] = 'v';
    printf("\t%c : .int 0\n", name);
}

/* parse and translate a data declaration */
void Decl(void)
{
    printf(".section .data\n");
    Match('v');
    Alloc(GetName());
}

/* parse and translate global declarations */
void TopDecls(void)
{
    char tmp_buf[MAX_BUF];
    while(Look != '.') {
        switch(Look) {
            case 'v':
                Decl();
                break;
            case 'p':
                DoProc();
                break;
            case 'P':
                DoMain();
                break;
            default:
                sprintf(tmp_buf, "Unrecognized keyword %c", Look);
                Abort(tmp_buf);
                break;
        }
        Fin();
    }
}

void Header()
{
    printf(".global _start\n");
}

void Prolog()
{
    EmitLn(".section .text");
    EmitLn("_start:");
}

void Epilog()
{
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
}

void DoProc(void)
{
    Match('p');
    char name = GetName();
    Fin();
    if (InTable(name)) {
        Duplicate(name);
    }
    ST[name-'A'] = 'p';
    PostLabel(name);
    BeginBlock();
    Return();
}

void Return()
{
    EmitLn("ret");
}

/* parse and translate a main program 
 * <main program> ::= PROGRAM <ident> <begin-block>
 * */
void DoMain(void)
{
    Match('P');
    char name = GetName();
    Fin();
    if (InTable(name)) {
        Duplicate(name);
    }
    Prolog();
    BeginBlock();
}

int main(int argc, char *argv[])
{
    Init();
    Header();
    TopDecls();
    Epilog();
    return 0;
}
