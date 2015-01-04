#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cradle.h"

#ifdef DEBUG
#define dprint(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define dprint(fmt, ...)
#endif

int main()
{
    Init();
    do {
        Scan();
        switch (Token) {
            case Ident:
                printf("--> Ident:");
                break;
            case Number:
                printf("--> Number:");
                break;
            case Operator:
                printf("--> Operator:");
                break;
            case IfSym:
            case ElseSym:
            case EndifSym:
            case EndSym:
                printf("--> keyword:");
                break;
        }
        printf("%s\n", Value);
    } while(Token != EndSym);

    return 0;
}
