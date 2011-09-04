#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF 100

static char tmp[MAX_BUF];

void GetChar() 
{
    Look = getchar();
}


void Error(char *s)
{
    printf("\nError: %s.", s);
}

void Abort(char *s)
{
    Error(s);
    exit(1);
}


void Expected(char *s)
{
    sprintf(tmp, "%s Expected", s);
    Abort(tmp);
}


void Match(char x)
{
    if(Look == x) {
        GetChar();
    } else {
        sprintf(tmp, "' %c ' ",  x);
        Expected(tmp);
    }
}


int IsAlpha(char c)
{
    return (UPCASE(c) >= 'A') && (UPCASE(c) <= 'Z');
} 

int IsDigit(char c)
{
    return (c >= '0') && (c <= '9');
}


char GetName()
{
    char c = Look;

    if( !IsAlpha(Look)) {
        sprintf(tmp, "Name");
        Expected(tmp);
    }

    GetChar();

    return UPCASE(c);
}


char GetNum()
{
    char c = Look;

    if( !IsDigit(Look)) {
        sprintf(tmp, "Integer");
        Expected(tmp);
    }

    GetChar();

    return c;
}

void Emit(char *s)
{
    printf("\t%s", s);
}

void EmitLn(char *s)
{
    Emit(s);
    printf("\n");
}

void Init()
{
    GetChar();
}

