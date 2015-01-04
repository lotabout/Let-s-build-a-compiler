#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 26
static int LCount = 0;
static char labelName[MAX_BUF];
/*static char identifier[MAX_BUF];*/
static int Table[TABLE_SIZE];
static char token_buf[MAX_BUF];        /* buffer to store token string */
static char tmp[MAX_BUF];

/* Helper Functions */
char uppercase(char c)
{
    if (IsAlpha(c)) {
        return (c & 0xDF);
    } else {
        return c;
    }
}

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

void Newline()
{
    if (Look == '\r') {
        GetChar();
        if (Look == '\n') {
            GetChar();
        }
    } else if (Look == '\n') {
        GetChar();
    }
}

int IsWhite(char c)
{
    return strchr(" \t\r\n", c) != NULL;
}

int IsOp(char c)
{
    return strchr("+-*/<>:=", c) != NULL;
}

int IsAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int IsDigit(char c)
{
    return (c >= '0') && (c <= '9');
}

int IsAddop(char c)
{
    return (c == '+') || (c == '-');
}

int IsBoolean(char c)
{
    return strchr("TF", uppercase(c)) != NULL;
}

int IsAlNum(char c)
{
    return IsAlpha(c) || IsDigit(c);
}

char *GetName()
{
    char *p = token_buf;

    if( !IsAlpha(Look)) {
        sprintf(tmp, "Name");
        Expected(tmp);
    }

    while(IsAlNum(Look)) {
        *p++ = uppercase(Look);
        GetChar();
    }
    SkipWhite();
    *p = '\0';

    return token_buf;
}


char *GetNum()
{
    char *p = token_buf;
    if( !IsDigit(Look)) {
        sprintf(tmp, "Integer");
        Expected(tmp);
    }

    while (IsDigit(Look)) {
        *p++ = Look;
        GetChar();
    }
    SkipWhite();
    *p = '\0';

    return token_buf;
}

char *GetOp()
{
    char *p = token_buf;
    if( !IsOp(Look)) {
        sprintf(tmp, "Operator");
        Expected(tmp);
    }

    while (IsOp(Look)) {
        *p++ = Look;
        GetChar();
    }
    SkipWhite();
    *p = '\0';

    return token_buf;
}

int GetBoolean()
{
    if (!IsBoolean(Look)) {
        Expected("Boolean Literal");
    }
    int ret = uppercase(Look) == 'T';
    GetChar();
    return ret;
}

int IsOrop(char c)
{
    return strchr("|~", c) != NULL;
}

int IsRelop(char c)
{
    return strchr("=#<>", c) != NULL;
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
    LCount = 0;

    InitTable();
    GetChar();
}

void SkipWhite()
{
    while (IsWhite(Look)) {
        GetChar();
    }
}

void InitTable()
{
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        Table[i] = 0;
    }

}

char *NewLabel()
{
    sprintf(labelName, "L%02d", LCount);
    LCount ++;
    return labelName;
}

void PostLabel(char *label)
{
    printf("%s:\n", label);
}

void Fin()
{
    if (Look == '\r') {
        GetChar();
    }
    if (Look == '\n') {
        GetChar();
    }
}
