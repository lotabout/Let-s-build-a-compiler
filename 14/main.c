#include <stdio.h>
#include <string.h>

#include "cradle.h"

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

/* parse and tranlate an expression
 * vestigial version */
char Expression()
{
    return Load(GetName());
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
