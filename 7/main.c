#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

#ifdef DEBUG
#define dprint(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define dprint(fmt, ...)
#endif

static char scan_buf[MAX_BUF];

char *Scan()
{
    char *ret = scan_buf;
    if (IsAlpha(Look)) {
        strcpy(ret, GetName());
    } else if (IsDigit(Look)) {
        strcpy(ret, GetNum());
    } else if (IsOp(Look)) {
        strcpy(ret, GetOp());
    } else {
        ret[0] = Look;
        ret[1] = '\0';
    }
    SkipWhite();

    return ret;
}

int main()
{
    char *token;
    Init();
    do {
        token = Scan();
        printf("==> %s\n", token);
        if (strcmp(token, "\r") == 0) {
            Fin();
        }
    } while(strcmp(token, ".") != 0);
    return 0;
}
