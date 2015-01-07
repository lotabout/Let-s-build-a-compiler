#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cradle.h"
#include <malloc.h>

#define MaxEntry 100
#define MAX_SYMBOL_LENGTH 10
static int LCount = 0;
static char labelName[MAX_BUF];
char tmp[MAX_BUF];

/*char ST[TABLE_SIZE];*/
static int NEntry = 0;
const char *ST[MaxEntry];
char SType[MaxEntry];


/* Keywords symbol table */
const char const *KWList[] = {
    "IF",
    "ELSE",
    "ENDIF",
    "WHILE",
    "ENDWHILE",
    "VAR",
    "END",
};
const char KWCode[] = "xileweve";
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

int Locate(char *symbol)
{
    return Lookup(ST, symbol, NEntry);
}

/* Add a new entry to symbol table */
void AddEntry(char *symbol, char type)
{
    CheckDup(symbol);
    if (NEntry == MaxEntry) {
        Abort("Symbol Table Full");
    }

    char *new_entry = (char *)malloc((strlen(symbol)+1)*sizeof(*new_entry));
    if (new_entry == NULL) {
        Abort("AddEntry: not enough memory allocating new_entry.");
    }
    strcpy(new_entry, symbol);
    ST[NEntry] = new_entry;
    SType[NEntry] = type;

    NEntry++;
}

/* Get an Identifier and Scan it for keywords */
void Scan()
{
    if (Token == 'x') {
        int index = Lookup(KWList, Value, KWNum);
        Token = KWCode[index+1];
    }
}

void MatchString(char *str)
{
    if (strcmp(Value, str) != 0) {
        sprintf(tmp, "\"%s\"", str);
        Expected(tmp);
    }
    Next();
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
    return strchr(" \t\r\n{", c) != NULL;
}

int IsAlNum(char c)
{
    return IsAlpha(c) || IsDigit(c);
}

void GetName()
{
    SkipWhite();
    if( !IsAlpha(Look)) {
        Expected("Name");
    }

    Token = 'x';
    char *p = Value;
    do {
        *p++ = uppercase(Look);
        GetChar();
    } while(IsAlNum(Look)) ;
    *p = '\0';
}

void GetNum()
{
    SkipWhite();
    if( !IsDigit(Look)) {
        Expected("Integer");
    }

    Token = '#';
    char *p = Value;
    do {
        *p++ = Look;
        GetChar();
    } while (IsDigit(Look));
    *p = '\0';
}

/* Get an operator */
void GetOp()
{
    SkipWhite();
    Token = Look;
    Value[0] = Look;
    Value[1] = '\0';
    GetChar();
}

/* Get the next input token */
void Next()
{
    SkipWhite();
    if (IsAlpha(Look)) {
        GetName();
    } else if (IsDigit(Look)) {
        GetNum();
    } else {
        GetOp();
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
    LCount = 0;

    InitTable();
    GetChar();
    Next();
}

void InitTable()
{
    int i;
    for (i = 0; i < MaxEntry; i++) {
        ST[i] = NULL;
        SType[i] = ' ';
    }

}

/* look for symbol in table */
bool InTable(char *symbol)
{
    return Locate(symbol) != -1;
}

/* Check to see if an identifier is in the symbol table,
 * report an error if it's not */
void CheckTable(char *symbol)
{
    if (! InTable(symbol)) {
        Undefined(symbol);
    }
}

void CheckDup(char *symbol)
{
    if (InTable(symbol)) {
        Duplicate(symbol);
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

void SkipWhite()
{
    while (IsWhite(Look)) {
        if (Look == '{') {
            SkipComment();
        } else {
            GetChar();
        }
    }
}

void SkipComment()
{
    while(Look != '}') {
        GetChar();
        if (Look == '{') {
            SkipComment();
        }
    }
    GetChar();
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

void LoadConst(char *value)
{
    sprintf(tmp, "movl $%s, %%eax", value);
    EmitLn(tmp);
}

/* Load a variable to primary register */
void LoadVar(char *name)
{
    if (!InTable(name)) {
        char name_string[MAX_BUF];
        Undefined(name_string);
    }
    sprintf(tmp, "movl %s, %%eax", name);
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
void Store(char *name)
{
    if (!InTable(name)) {
        char name_string[MAX_BUF];
        Undefined(name_string);
    }
    sprintf(tmp, "movl %%eax, %s", name);
    EmitLn(tmp);
}

void Undefined(char *name)
{
    sprintf(tmp, "Undefined Identifier: %s", name);
    Abort(tmp);
}

void Duplicate(char *name)
{
    sprintf(tmp, "Duplicate Identifier: %s", name);
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

/* set %eax if Compare was >= */
void SetGreaterOrEqual()
{
    EmitLn("setle %al");
    EmitLn("movsx %al, %eax");
}

/* set %eax if Compare was < */
void SetLess()
{
    EmitLn("setg %al");
    EmitLn("movsx %al, %eax");
}

/* set %eax if Compare was <= */
void SetLessOrEqual()
{
    EmitLn("setge %al");
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
