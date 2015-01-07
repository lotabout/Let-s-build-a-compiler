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
void Undefined(char name)
{
    sprintf(tmp, "Undefined Identifier: %c", name);
    Abort(tmp);
}

/* report an duplicate identifier */
void Duplicate(char name)
{
    sprintf(tmp, "Duplicate Identifier: %c", name);
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
    return ST[symbol - 'A'] != '?';
}

/* add a new symbol to table */
void AddEntry(char symbol, char type)
{
    CheckDup(symbol);
    ST[symbol-'A'] = type;
}

/* check an entry to make sure it's a variable */
void CheckVar(char name)
{
    char tmp_buf[MAX_BUF];
    if (!InTable(name)) {
        Undefined(name);
    }
    if (TypeOf(name) != 'v') {
        sprintf(tmp_buf, "%c is not a variable", name);
        Abort(tmp_buf);
    }
}

/* check for a duplicate variable name */
void CheckDup(char name)
{
    if (InTable(name)) {
        Duplicate(name);
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

bool IsVarType(char c)
{
    return strchr("BWLbwl", c) != NULL;
}

/* get a variable type from the symbol table */
char VarType(char name)
{
    char type = TypeOf(name);
    if (!IsVarType(type)) {
        sprintf(tmp, "Identifier %c is not a variable", name);
    }
    return type;
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
void LoadVar(char name, char type)
{
    char src[MAX_BUF];
    src[0] = name;
    src[1] = '\0';
    char *dst;
    switch(type) {
        case 'B':
            dst = "%al";
            break;
        case 'W':
            dst = "%ax";
            break;
        case 'L':
            dst = "%eax";
            break;
        default:
            dst = "%eax";
            break;
    }
    Move(type, src, dst);
}

void Move(char size, char *src, char *dest)
{
    sprintf(tmp, "MOV%c %s, %s", size, src, dest);
    EmitLn(tmp);
}

/* store the primary register */
void StoreVar(char name, char type)
{
    char dest[MAX_BUF];
    dest[0] = name;
    dest[1] = '\0';
    char *src;
    switch(type) {
        case 'B':
            src = "%al";
            break;
        case 'W':
            src = "%ax";
            break;
        case 'L':
            src = "%eax";
            break;
        default:
            src = "%eax";
            break;
    }
    Move(type, src, dest);
}

/* load a variable to the primary register */
void Load(char name)
{
    LoadVar(name, VarType(name));
}

/* store a variable from the primary register */
void Store(char name)
{
    StoreVar(name, VarType(name));
}

/* initialize the symbol table */
void InitTable(void)
{
    int i;
    for (i = 0; i < MaxEntry; ++i) {
        ST[i] = '?';
    }
}

/* Dump the symbol table */
void DumpTable()
{
    int i;
    for (i = 0; i < MaxEntry; ++i) {
        if (ST[i] != '?') {
            printf("%c: %c\n", i+'A', ST[i]);
        }
    }
}

/* initialize */
void Init()
{
    GetChar();
    SkipWhite();
    InitTable();
}
