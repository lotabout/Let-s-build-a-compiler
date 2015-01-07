#ifndef _CRADLE_H
#define _CRADLE_H

#include <stdbool.h>

#define MAX_BUF 100
extern const char TAB;
extern const char CR;
extern const char LF;

extern char Look;   /* lookahead character */
extern char ST[];   /* symbol table */
extern int Params[];    /* parameter table */
extern int NumParams;
extern int Base;

/* read new character from input stream */
void GetChar();

/* Report an Error */
void Error(char *str);

/* report Error and Halt */
void Abort(char *str);

/* report what was expected */
void Expected(char *str);

/* report an undefined identifier */
void Undefined(char symbol);

/* report an duplicate identifier */
void Duplicate(char symbol);

/* Get type of symbole */
char TypeOf(char symbol);

/* check if a symbol is in table */
bool InTable(char symbol);

/* add a new symbol to table */
void AddEntry(char symbol, char type);

/* check an entry to make sure it's a variable */
void CheckVar(char name);


bool IsAlpha(char c);
bool IsDigit(char c);
bool IsAlNum(char c);
bool IsAddop(char c);
bool IsMulop(char c);
bool IsRelop(char c);
bool IsWhite(char c);

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
void PostLabel(char label);

/* Load a variable to the primary register */
void LoadVar(char name);

/* store the primary register */
void StoreVar(char name);

/* load a parameter to the primary register */
void LoadParam(int n);

/* store a parameter from the primary register */
void StoreParam(int n);

/* push the primary register to the stack */
void Push();

/* Adjust the stack pointer upwards by n bytes */
void CleanStack(int bytes);

/* initialize the symbol table */
void InitTable(void);

/* initialize parameter table to NULL */
void ClearParams();

/* find the parameter number */
int ParamNumber(char name);

/* see if an identifier is a parameter */
bool IsParam(char name);

/* Add a new parameter to table */
void AddParam(char name);

/* initialize */
void Init(void);

#endif
