#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"


#define MaxEntry 26
const char TAB = '\t';
const char CR = '\n';
const char LF = '\r';

char tmp[MAX_BUF];  /* temporary buffer */

char Look;
char ST[MaxEntry];   /* symbol table */

/* read new character from input stream */
void GetChar()
{
    Look = getchar();
}

/* Report an Error */
void Error(char *str)
{
    printf("\n");
    printf("\aError: %s.\n", str);
}

/* report Error and Halt */
void Abort(char *str)
{
    Error(str);
    exit(1);
}

/* report what was expected */
void Expected(char *str)
{
    sprintf(tmp, "Expected: %s", str);
    Abort(tmp);
}

/* report an undefined identifier */
void Undefined(char *str)
{
    sprintf(tmp, "Undefined Identifier: %s", str);
    Abort(tmp);
}

/* report an duplicate identifier */
void Duplicate(char *str)
{
    sprintf(tmp, "Duplicate Identifier: %s", str);
    Abort(tmp);
}

/* Get type of symbole */
char TypeOf(char symbol)
{
    return ST[symbol - 'A'];
}

/* check if a symbol is in table */
bool InTable(char symbol)
{
    return ST[symbol - 'A'] != ' ';
}

/* add a new symbol to table */
void AddEntry(char symbol, char type)
{
    char tmp_buf[MAX_BUF];
    if (InTable(symbol)) {
        tmp_buf[0] = symbol;
        tmp_buf[1] = '\0';
        Duplicate(tmp_buf);
    }
    ST[symbol-'A'] = type;
}

/* check an entry to make sure it's a variable */
void CheckVar(char name)
{
    char tmp_buf[MAX_BUF];
    if (!InTable(name)) {
        tmp_buf[0] = name;
        tmp_buf[1] = '\0';
        Undefined(tmp_buf);
    }
    if (TypeOf(name) != 'v') {
        sprintf(tmp_buf, "%c is not a variable", name);
        Abort(tmp_buf);
    }
}

/* turn an character into uppercase */
char upcase(char c)
{
    return (c & 0xDF);
}

bool IsAlpha(char c)
{
    char upper = upcase(c);
    return (upper >= 'A') && (upper <= 'Z');
}

bool IsDigit(char c)
{
    return (c >= '0') && (c <= '9');
}

bool IsAlNum(char c)
{
    return IsAlpha(c) || IsDigit(c);
}

bool IsAddop(char c)
{
    return strchr("+-", c) != NULL;
}

bool IsMulop(char c)
{
    return strchr("*/", c) != NULL;
}

bool IsRelop(char c)
{
    return strchr("=#<>", c) != NULL;
}

bool IsWhite(char c)
{
    return strchr(" \t", c) != NULL;
}

/* skip over leading white space */
void SkipWhite(void)
{
    while(IsWhite(Look)) {
        GetChar();
    }
}

/* skip over an End-Of-Line */
void Fin(void)
{
    if (Look == CR) {
        GetChar();
        if (Look == LF) {
            GetChar();
        }
    } else if (Look == LF){
        GetChar();
    }
}

/* match a specific input character */
void Match(char c)
{
    if (Look == c) {
        GetChar();
    } else {
        char tmp_buf[MAX_BUF];
        sprintf(tmp_buf, "'%c'", c);
        Expected(tmp_buf);
    }
    SkipWhite();
}

/* Get an identifier */
char GetName(void)
{
    if (! IsAlpha(Look)) {
        Expected("Name");
    }
    char name = upcase(Look);
    GetChar();
    SkipWhite();
    return name;
}

/* Get a number */
char GetNum(void)
{
    if (!IsDigit(Look)) {
        Expected("Integer");
    }
    char num = Look;
    GetChar();
    SkipWhite();
    return num;
}

/* output a string with TAB */
void Emit(char *str)
{
    printf("\t%s", str);
}

/* Output a string with TAB and CRLF */
void EmitLn(char *str)
{
    Emit(str);
    printf("\n");
}

/* Post a label to output */
void PostLabel(char *label)
{
    printf("%s:\n", label);
}

/* Load a variable to the primary register */
void LoadVar(char name)
{
    CheckVar(name);
    sprintf(tmp, "movl %c, %%eax", name);
    EmitLn(tmp);
}

/* store the primary register */
void StoreVar(char name)
{
    CheckVar(name);
    sprintf(tmp, "movl %%eax, %c", name);
    EmitLn(tmp);
}

/* initialize the symbol table */
void InitTable(void)
{
    int i;
    for (i = 0; i < MaxEntry; ++i) {
        ST[i] = ' ';
    }
}

/* initialize */
void Init()
{
    GetChar();
    SkipWhite();
    InitTable();
}
