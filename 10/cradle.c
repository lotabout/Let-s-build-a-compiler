#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 26
static int LCount = 0;
static char labelName[MAX_BUF];
char tmp[MAX_BUF];

static int Table[TABLE_SIZE];

/* Helper Functions */
char uppercase(char c)
{
    return (c & 0xDF);
}

void GetChar() 
{
    Look = getchar();
    /* printf("Getchar: %c\n", Look); */
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

char GetName()
{
    char c = Look;

    if( !IsAlpha(Look)) {
        sprintf(tmp, "Name");
        Expected(tmp);
    }

    GetChar();

    return uppercase(c);
}


int GetNum()
{
    int value = 0;
    if( !IsDigit(Look)) {
        sprintf(tmp, "Integer");
        Expected(tmp);
    }

    while (IsDigit(Look)) {
        value = value * 10 + Look - '0';
        GetChar();
    }

    return value;
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
