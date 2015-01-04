#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 26
static int LCount = 0;
static char labelName[MAX_BUF];
/*static char identifier[MAX_BUF];*/
static int Table[TABLE_SIZE];
char tmp[MAX_BUF];


/* Keywords symbol table */
const char const *KWList[] = {
    "IF",
    "ELSE",
    "ENDIF",
    "END",
};
const char KWCode[] = "xilee";
const int KWNum = sizeof(KWList)/sizeof(*KWList);

char Token;      /* current token */
char Value[MAX_BUF];     /* string token of Look */

/* Table Lookup
 * If the input string matches a table entry, return the entry index, else
 * return -1.
 * *n* is the size of the table */
int Lookup(const char const *table[], const char *string, int n)
{
    int i;
    bool found = false;

    for (i = 0; i < n; ++i) {
        if (strcmp(table[i], string) == 0) {
            found = true;
            break;
        }
    }
    return found ? i : -1;
}


void Scan()
{ 
    /* in Unix/Linux, Endline is CR instead of LF CR in MSDOS*/
    SkipWhite();   
    while(Look == '\n') {
        Fin();
    }

    GetName();
    int index = Lookup(KWList, Value, KWNum);
    Token = KWCode[index+1];
}

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

void MatchString(char *str)
{
    if (strcmp(Value, str) != 0) {
        sprintf(tmp, "\"%s\"", Value);
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

void GetName()
{
    SkipWhite();   
    while(Look == '\n') {
        Fin();
    }

    char *p = Value;
    if (!IsAlpha(Look)) {
        Expected("Name");
    }

    while(IsAlNum(Look)) {
        *p++ = uppercase(Look);
        GetChar();
    }
    *p = '\0';
}

void GetNum()
{
    SkipWhite();
    char *p = Value;
    if( !IsDigit(Look)) {
        Expected("Integer");
    }

    while (IsDigit(Look)) {
        *p++ = Look;
        GetChar();
    }
    *p = '\0';
    Token = '#';
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

