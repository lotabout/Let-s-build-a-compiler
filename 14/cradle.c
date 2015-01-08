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
int GetNum(void)
{
    if (!IsDigit(Look)) {
        Expected("Integer");
    }
    int val = 0;
    while(IsDigit(Look)) {
        val = 10*val + Look - '0';
        GetChar();
    }
    SkipWhite();
    return val;
}

/* load a constant to the primary register */
char LoadNum(int val)
{
    char type;
    if (abs(val) <= 127) {
        type = 'B';
    } else if (abs(val) <= 32767) {
        type = 'W';
    } else {
        type = 'L';
    }
    LoadConst(val, type);
    return type;
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
char Load(char name)
{
    char type = VarType(name);
    LoadVar(name, type);
    return type;
}

/* Load a constant to the primary register */
void LoadConst(int val, char type)
{
    char src[MAX_BUF];
    sprintf(src, "$%d", val);
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


/* store a variable from the primary register */
void Store(char name, char src_type)
{
    char dst_type = VarType(name);
    Convert(src_type, dst_type, 'a');
    StoreVar(name, dst_type);
}

/* convert a data item from one type to another */
void Convert(char src, char dst, char reg)
{
    /* this function only works when storing a variable and
     * (B,W) -> (W,L)
     * and the action are the same: zero extend %eax */
    char tmp_buf[MAX_BUF];
    if (src != dst) {
        switch(src) {
            case 'B':
                sprintf(tmp_buf, "movzx %%%cl, %%e%cx", reg, reg);
                EmitLn(tmp_buf);
                break;
            case 'W':
                sprintf(tmp_buf, "movzx %%%cx, %%e%cx", reg, reg);
                EmitLn(tmp_buf);
                break;
            default:
                break;
        }
    }
}

/* promote the size of a register value */
char Promote(char src_type, char dst_type, char reg)
{
    char type = src_type;
    if (src_type != dst_type) {
        if ((src_type == 'B') || ((src_type == 'W' && dst_type == 'L'))) {
            Convert(src_type, dst_type, reg);
            type = dst_type;
        }
    }
    return type;
}

/* force both arguments to same type */
char SameType(char src_type, char dst_type)
{
    src_type = Promote(src_type, dst_type, 'd');
    return Promote(dst_type, src_type, 'a');
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

void Clear()
{
    EmitLn("xor %eax, %eax");
}

/* Push Primary onto stack */
void Push(char type)
{
    switch(type) {
        case 'B':
        case 'W':
            EmitLn("pushw %ax");
            break;
        case 'L':
            EmitLn("pushl %eax");
            break;
        default:
            break;
    }
}

void Pop(char type)
{
    switch(type) {
        case 'B':
        case 'W':
            EmitLn("popw %dx");
            break;
        case 'L':
            EmitLn("popl %edx");
            break;
        default:
            break;
    }
}

/* Add Top of Stack to primary */
char PopAdd(char src_type, char dst_type)
{
    Pop(src_type);
    dst_type = SameType(src_type, dst_type);
    GenAdd(dst_type);
    return dst_type;

    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
}

/* Subtract Primary from Top of Stack */
char PopSub(char src_type, char dst_type)
{
    Pop(src_type);
    dst_type = SameType(src_type, dst_type);
    GenSub(dst_type);
    return dst_type;

    EmitLn("subl (%esp), %eax");
    EmitLn("neg %eax");
    EmitLn("addl $4, %esp");
}

/* add top of stack to primary */
void GenAdd(char type)
{
    switch(type) {
        case 'B':
            EmitLn("addb %dl, %al");
            break;
        case 'W':
            EmitLn("addw %dx, %ax");
            break;
        case 'L':
            EmitLn("addl %edx, %eax");
            break;
        default:
            EmitLn("addl %edx, %eax");
            break;
    }
}

/* subtract primary from top of stack to */
void GenSub(char type)
{
    switch(type) {
        case 'B':
            EmitLn("subb %dl, %al");
            EmitLn("neg %al");
            break;
        case 'W':
            EmitLn("subw %dx, %ax");
            EmitLn("neg %ax");
            break;
        case 'L':
            EmitLn("subl %edx, %eax");
            EmitLn("neg %eax");
            break;
        default:
            EmitLn("subl %edx, %eax");
            EmitLn("neg %eax");
            break;
    }
}

/* multiply top of stack by primary (Word) */
void GenMul()
{
    EmitLn("imulw %dx, %ax");
}

/* multiply top of stack by primary (Long) */
void GenLongMul()
{
    EmitLn("imull %edx, %eax");
}

void GenDiv()
{
    EmitLn("Dividision not implemented yet!");
}

void GenLongDiv()
{
    EmitLn("Dividision not implemented yet!");
}

/* multiply top of stack by primary */
char PopMul(char src_type, char dst_type)
{
    Pop(src_type);
    char type = SameType(src_type, dst_type);
    Convert(type, 'W', 'd');
    Convert(type, 'W', 'a');
    if (type == 'L') {
        GenLongMul(type);
    } else {
        GenMul();
    }

    if (type == 'B') {
        type = 'W';
    } else {
        type = 'L';
    }
    return type;
}

/* divide top of stack by primary */
char PopDiv(char src_type, char dst_type)
{
    char type;
    Pop(src_type);
    Convert(src_type, 'L', 'd');
    if (src_type == 'L' || dst_type == 'L') {
        Convert(dst_type, 'L', 'a');
        GenLongDiv();
        type = 'L';
    } else {
        Convert(dst_type, 'w', 'a');
        GenDiv();
        type = src_type;
    }
    return type;
}
