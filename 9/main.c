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

int main()
{
    Init();
    Prog();
    return 0;
}
