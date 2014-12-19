#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>

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
        SkipWhite();
    } else {
        sprintf(tmp, "' %c ' ",  x);
        Expected(tmp);
    }
}


int IsAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
} 

int IsDigit(char c)
{
    return (c >= '0') && (c <= '9');
}

int IsAlNum(char c)
{
    return IsAlpha(c) || IsDigit(c);
}

int IsAddop(char c)
{
    return (c == '+') || (c == '-');
}

int IsWhite(char c)
{
    return (c == ' ') || (c == '\t');
}

char* GetName()
{
    char *token = token_buf;

    if( !IsAlNum(Look)) {
        Expected("Name");
    }
    while (IsAlNum(Look)) {
        *token = Look;
        token++;

        GetChar();
    }

    SkipWhite();

    *token = '\0';
    return token_buf;
}


char* GetNum()
{
    char *value = token_buf;

    if( !IsAlNum(Look)) {
        Expected("Integer");
    }
    while (IsDigit(Look)) {
        *value = Look;
        value++;

        GetChar();
    }

    SkipWhite();

    *value = '\0';
    return token_buf;
}

void SkipWhite()
{
    while (IsWhite(Look)) {
        GetChar();
    }
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
    SkipWhite();
}

