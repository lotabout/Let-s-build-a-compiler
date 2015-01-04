#ifndef _CRADLE_H
#define _CRADLE_H

#define MAX_BUF 100
char Look;

void GetChar();

void Error(char *s);
void Abort(char *s);
void Expected(char *s);
void Match(char x);

void Newline();

int IsWhite(char c);
int IsOp(char c);

int IsAlpha(char c);
int IsDigit(char c);
int IsAlNum(char c);
int IsAddop(char c);
int IsBoolean(char c);
int IsOrop(char c);
int IsRelop(char c);

char* GetName();
char *GetNum();
char *GetOp();
int GetBoolean();

void SkipWhite();

void Emit(char *s);
void EmitLn(char *s);

void Init();
void InitTable();

char *NewLabel();
void PostLabel(char *label);

void Fin();
#endif
