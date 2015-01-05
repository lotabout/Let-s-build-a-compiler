#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

#ifdef DEBUG
#define dprint(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define dprint(fmt, ...)
#endif

void Prolog(char name);
void Epilog(char name);
void Prog();
void DoBlock(char name);
void Declarations();
void Labels();
void Constants();
void Types();
void Variables();
void DoProcedure();
void DoFunction();
void Statements();

void Prog()
{
    Match('p');     /* handles program header part */
    char name = GetName();
    Prolog(name);
    DoBlock(name);
    Match('.');
    Epilog(name);
}

void Prolog(char name)
{
    EmitLn(".text");
    EmitLn(".global _start");
    EmitLn("_start:");
}

void Epilog(char name)
{
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
}
void DoBlock(char name)
{
    Declarations();
    sprintf(tmp, "%c", name);
    PostLabel(tmp);
    Statements();
}

void Declarations()
{
    while(strchr("lctvpf", Look) != NULL) {
        switch(Look) {
            case 'l':
                Labels();
                break;
            case 'c':
                Constants();
                break;
            case 't':
                Types();
                break;
            case 'v':
                Variables();
                break;
            case 'p':
                DoProcedure();
                break;
            case 'f':
                DoFunction();
            default:
                break;
        }
    }
}

void Labels()
{
    Match('l');
}

void Constants()
{
    Match('c');
}

void Types()
{
    Match('t');
}

void Variables()
{
    Match('v');
}

void DoProcedure()
{
    Match('p');
}

void DoFunction()
{
    Match('f');
}

void Statements()
{
    Match('b');
    while(Look != 'e') {
        GetChar();
    }
    Match('e');
}

int main()
{
    Init();
    Prog();
    return 0;
}
