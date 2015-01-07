#include <stdio.h>
#include <string.h>

#include "cradle.h"

void Expression();
void Assignment();
void DoBlock();
void BeginBlock();
void Alloc(char name);
void Decl(void);
void TopDecls(void);

void Header();
void Prolog();
void Epilog();

/* parse and tranlate an expression
 * vestigial version */
void Expression()
{
    LoadVar(GetName());
}

/* parse and tranlate an assignment statement */
void Assignment()
{
    char name = GetName();
    Match('=');
    Expression();
    StoreVar(name);
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

/* allocate storage for a variable */
void Alloc(char name)
{
    if (InTable(name)) {
        char tmp_buf[MAX_BUF];
        tmp_buf[0] = name;
        tmp_buf[1] = '\0';
        Duplicate(tmp_buf);
    }
    ST[name-'A'] = 'v';
    printf("\t%c : .int 0\n", name);
}

/* parse and translate a data declaration */
void Decl(void)
{
    Match('v');
    Alloc(GetName());
}

/* parse and translate global declarations */
void TopDecls(void)
{
    printf(".section .data\n");
    char tmp_buf[MAX_BUF];
    while(Look != 'b') {
        switch(Look) {
            case 'v':
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
    Header();
    TopDecls();
    Prolog();
    BeginBlock();
    Epilog();
    return 0;
}
