#ifndef _CRADLE_H
#define _CRADLE_H

#include <stdbool.h>

#define MAX_BUF 100
extern const char TAB;
extern const char CR;
extern const char LF;

extern char Look;   /* lookahead character */
extern char ST[];   /* symbol table */

/* read new character from input stream */
void GetChar();

/* Report an Error */
void Error(char *str);

/* report Error and Halt */
void Abort(char *str);

/* report what was expected */
void Expected(char *str);

/* report an undefined identifier */
void Undefined(char name);

/* report an duplicate identifier */
void Duplicate(char name);

/* Get type of symbole */
char TypeOf(char symbol);

/* check if a symbol is in table */
bool InTable(char symbol);

/* Dump the symbol table */
void DumpTable();

/* add a new symbol to table */
void AddEntry(char symbol, char type);

/* check an entry to make sure it's a variable */
void CheckVar(char name);

/* check for a duplicate variable name */
void CheckDup(char name);


bool IsAlpha(char c);
bool IsDigit(char c);
bool IsAlNum(char c);
bool IsAddop(char c);
bool IsMulop(char c);
bool IsRelop(char c);
bool IsWhite(char c);
bool IsVarType(char c);

/* skip over leading white space */
void SkipWhite(void);
/* skip over an End-Of-Line */
void Fin(void);

/* match a specific input character */
void Match(char c);

/* Get an identifier */
char GetName(void);

/* Get a number */
char GetNum(void);

/* output a string with TAB */
void Emit(char *str);
/* Output a string with TAB and CRLF */
void EmitLn(char *str);


/* Post a label to output */
void PostLabel(char *label);

/* Load a variable to the primary register */
void LoadVar(char name, char type);

/* store the primary register */
void StoreVar(char name, char type);

/* initialize the symbol table */
void InitTable(void);

/* initialize */
void Init(void);

/* get a variable type from the symbol table */
char VarType(char name);

void Move(char size, char *src, char *dest);

/* load a variable to the primary register */
char Load(char name);
/* store a variable from the primary register */
void Store(char name, char src_type);

/* convert a data item from one type to another */
void Convert(char src, char dst);


#endif
