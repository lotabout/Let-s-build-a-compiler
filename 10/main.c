#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

#ifdef DEBUG
#define dprint(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define dprint(fmt, ...)
#endif

void Prog();
void Prolog();
void Epilog();
void Header();
void Main();

void Prog()
{
    Match('p');     /* handles program header part */
    Header();
    Main();
    Match('.');
}

void Header()
{
    EmitLn(".text");
    EmitLn(".global _start");
}

void Prolog()
{
    EmitLn("_start:");
}

void Epilog()
{
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
}

void Main()
{
    Match('b');
    Prolog();
    Match('e');
    Epilog();
}

int main()
{
    Init();
    Prog();
    if (Look != '\n') {
        Abort("Unexpected data after '.'");
    }
    return 0;
}
