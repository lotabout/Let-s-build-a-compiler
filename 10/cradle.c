#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cradle.h"

#define TABLE_SIZE 26
static int LCount = 0;
static char labelName[MAX_BUF];
char tmp[MAX_BUF];

char ST[TABLE_SIZE];

/* Keywords symbol table */
const char const *KWList[] = {
    "IF",
    "ELSE",
    "ENDIF",
    "WHILE",
    "ENDWHILE",
    "VAR",
    "BEGIN",
    "END",
    "PROGRAM"
};
const char KWCode[] = "xilewevbep";
const int KWNum = sizeof(KWList)/sizeof(*KWList);

char Token;             /* current token */
char Value[MAX_BUF];    /* string token of Look */

/* Helper Functions */
char uppercase(char c)
{
    if (IsAlpha(c)) {
        return (c & 0xDF);
    } else {
        return c;
    }
}

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

/* Get an Identifier and Scan it for keywords */
void Scan()
{ 
    GetName();
    int index = Lookup(KWList, Value, KWNum);
    Token = KWCode[index+1];
}

void MatchString(char *str)
{
    if (strcmp(Value, str) != 0) {
        sprintf(tmp, "\"%s\"", Value);
        Expected(tmp);
    }
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
    NewLine();
    if(Look == x) {
        GetChar();
    } else {
        sprintf(tmp, "' %c ' ",  x);
        Expected(tmp);
    }
    SkipWhite();
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

int IsMulop(char c)
{
    return (c == '*') || (c == '/');
}

int IsOrOp(char c)
{
    return strchr("|~", c) != NULL;
}

int IsRelop(char c)
{
    return strchr("=#<>", c) != NULL;
}

int IsWhite(char c)
{
    return strchr(" \t\r\n", c) != NULL;
}

int IsAlNum(char c)
{
    return IsAlpha(c) || IsDigit(c);
}

void GetName()
{
    NewLine();
    if( !IsAlpha(Look)) {
        Expected("Name");
    }

    char *p = Value;
    while(IsAlNum(Look)) {
        *p++ = uppercase(Look);
        GetChar();
    }
    *p = '\0';
    SkipWhite();
}


int GetNum()
{
    NewLine();
    int value = 0;
    if( !IsDigit(Look)) {
        sprintf(tmp, "Integer");
        Expected(tmp);
    }

    while (IsDigit(Look)) {
        value = value * 10 + Look - '0';
        GetChar();
    }

    SkipWhite();

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
    Scan();
    SkipWhite();
}

void InitTable()
{
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        ST[i] = ' ';
    }

}

bool InTable(char name)
{
    return ST[name - 'A'] != ' ';
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

void SkipWhite()
{
    while (IsWhite(Look)) {
        GetChar();
    }
}

/* Skip over an End-of-Line */
void NewLine()
{
    while(Look == '\n') {
        GetChar();
        if (Look == '\r') {
            GetChar();
        }
        SkipWhite();
    }
}

/* re-targetable routines */
void Clear()
{
    EmitLn("xor %eax, %eax");
}

void Negate()
{
    EmitLn("neg %eax");
}

void LoadConst(int n)
{
    sprintf(tmp, "movl $%d, %%eax", n);
    EmitLn(tmp);
}

/* Load a variable to primary register */
void LoadVar(char name)
{
    if (!InTable(name)) {
        char name_string[MAX_BUF];
        name_string[0] = name;
        name_string[1] = '\0';
        Undefined(name_string);
    }
    sprintf(tmp, "movl %c, %%eax", name);
    EmitLn(tmp);
}


/* Push Primary onto stack */
void Push()
{
    EmitLn("pushl %eax");
}

/* Add Top of Stack to primary */
void PopAdd()
{
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
}

/* Subtract Primary from Top of Stack */
void PopSub()
{
    EmitLn("subl (%esp), %eax");
    EmitLn("neg %eax");
    EmitLn("addl $4, %esp");
}

/* multiply top of stack by primary */
void PopMul()
{
    EmitLn("imull (%esp), %eax");
    EmitLn("addl $4, %esp");
}

/* divide top of stack by primary */
void PopDiv()
{
    /* for a expersion like a/b we have eax=b and %(esp)=a
     * but we need eax=a, and b on the stack
     */
    EmitLn("movl (%esp), %edx");
    EmitLn("addl $4, %esp");
    EmitLn("pushl %eax");
    EmitLn("movl %edx, %eax");

    /* sign extesnion */
    EmitLn("sarl $31, %edx");
    EmitLn("idivl (%esp)");
    EmitLn("addl $4, %esp");
}

/* store primary to variable */
void Store(char name)
{
    if (!InTable(name)) {
        char name_string[MAX_BUF];
        name_string[0] = name;
        name_string[1] = '\0';
        Undefined(name_string);
    }
    sprintf(tmp, "movl %%eax, %c", name);
    EmitLn(tmp);
}

void Undefined(char *name)
{
    sprintf(tmp, "Undefined Identifier: %s", name);
    Abort(tmp);
}

/* Complement the primary register */
void NotIt()
{
    EmitLn("not %eax");
}

/* AND top of Stack with primary */
void PopAnd()
{
    EmitLn("and (%esp), %eax");
    EmitLn("addl $4, %esp");
}

/* OR top of Stack with primary */
void PopOr()
{
    EmitLn("or (%esp), %eax");
    EmitLn("addl $4, %esp");
}

/* XOR top of Stack with primary */
void PopXor()
{
    EmitLn("xor (%esp), %eax");
    EmitLn("addl $4, %esp");
}

/* Compare top of Stack with primary */
void PopCompare()
{
    EmitLn("addl $4, %esp");
    EmitLn("cmp -4(%esp), %eax");
}

/* set %eax if Compare was = */
void SetEqual()
{
    EmitLn("sete %al");
    EmitLn("movsx %al, %eax");
}

/* set %eax if Compare was != */
void SetNEqual()
{
    EmitLn("setne %al");
    EmitLn("movsx %al, %eax");
}

/* set %eax if Compare was > */
void SetGreater()
{
    EmitLn("setl %al");
    EmitLn("movsx %al, %eax");
}

/* set %eax if Compare was < */
void SetLess()
{
    EmitLn("setg %al");
    EmitLn("movsx %al, %eax");
}

/* Branch unconditional */
void Branch(char *label)
{
    sprintf(tmp, "jmp %s", label);
    EmitLn(tmp);
}

/* Branch False */
void BranchFalse(char *label)
{
    EmitLn("test $1, %eax");
    sprintf(tmp, "jz %s", label);
    EmitLn(tmp);
}
