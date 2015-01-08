#include <stdio.h>
#include <string.h>

#include "cradle.h"

char Term();
char Expression();
void Assignment();
void DoBlock();
void BeginBlock();
void Alloc(char name, char type);
void Decl(void);
void TopDecls(void);

void Header();
void Prolog();
void Epilog();

void Block();

char Unop();
char Add(char type);
char Subtract(char type);

/* parse and tranlate an expression
 * vestigial version */
char Expression()
{
    char type;
    if (IsAddop(Look)) {
        type = Unop();
    } else {
        type = Term();
    }

    while(IsAddop(Look)) {
        Push(type);
        switch (Look) {
            case '+':
                type = Add(type);
                break;
            case '-':
                type = Subtract(type);
                break;
            default:
                break;
        }
    }
    return type;
}

char Term()
{
    if (IsAlpha(Look)) {
        return Load(GetName());
    } else {
        return LoadNum(GetNum());
    }
}

/* process a term with leading unary operator */
char Unop()
{
    Clear();
    return 'W';
}

char Add(char type)
{
    Match('+');
    return PopAdd(type, Term());
}

char Subtract(char type)
{
    Match('-');
    return PopSub(type, Term());
}

/* parse and tranlate an assignment statement */
void Assignment()
{
    char name = GetName();
    Match('=');
    Store(name, Expression());
}

void Block()
{
    while(Look != '.') {
        Assignment();
        Fin();
    }
}

/* parse and translate a block of statement */
void DoBlock()
{
    while(strchr("e", Look) == NULL) {
        Assignment();
        Fin();
    }
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


/* Generate code for allocation of a variable */
void AllocVar(char name, char type)
{
    char *p = "";
    switch(type) {
        case 'B':
            p = "byte";
            break;
        case 'W':
            p = "word";
            break;
        case 'L':
            p = "long";
            break;
        default:
            break;
    }
    printf("%c:\t.%s 0\n", name, p);
}

/* allocate storage for a variable */
void Alloc(char name, char type)
{
    AddEntry(name, type);
    AllocVar(name, type);
}

/* parse and translate a data declaration */
void Decl(void)
{
    char type = GetName();
    Alloc(GetName(), type);
}

/* parse and translate global declarations */
void TopDecls(void)
{
    printf(".section .data\n");
    char tmp_buf[MAX_BUF];
    while(Look != 'B') {
        switch(Look) {
            case 'b':
            case 'w':
            case 'l':
                Decl();
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

int main(int argc, char *argv[])
{
    Init();
    TopDecls();
    Match('B');
    Fin();
    Block();
    DumpTable();
    return 0;
}
