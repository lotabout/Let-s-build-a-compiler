#ifndef _CRADLE_H
#define _CRADLE_H
#include <stdbool.h>

#define MAX_BUF 100
extern char tmp[MAX_BUF];
extern char ST[];
extern char Token;
extern char Value[MAX_BUF];
char Look;

void GetChar();

void Error(char *s);
void Abort(char *s);
void Expected(char *s);
void Match(char x);
void MatchString(char *str);

int IsAlpha(char c);
int IsDigit(char c);
int IsAddop(char c);
int IsMulop(char c);
int IsOrOp(char c);
int IsRelop(char c);
int IsWhite(char c);
int IsAlNum(char c);

void GetName();
int GetNum();

void Emit(char *s);
void EmitLn(char *s);

void Init();
void InitTable();
bool InTable(char name);

char *NewLabel();
void PostLabel(char *label);
void SkipWhite();
void NewLine();
void Scan();

/* re-targetable routines */
void Clear();
void Negate();
void LoadConst(int n);
void LoadVar(char name);
void Push();
void PopAdd();
void PopSub();
void PopMul();
void PopDiv();
void Store(char name);
void Undefined(char *name);
void NotIt();
void PopAnd();
void PopOr();
void PopXor();
void PopCompare();
void SetEqual();
void SetNEqual();
void SetGreater();
void SetLess();
void Branch(char *label);
void BranchFalse(char *label);

#endif
