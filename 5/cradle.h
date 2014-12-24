#ifndef _CRADLE_H
#define _CRADLE_H

#define MAX_BUF 100
static char tmp[MAX_BUF];
char Look;

void GetChar();

void Error(char *s);
void Abort(char *s);
void Expected(char *s);
void Match(char x);

void Newline();

int IsAlpha(char c);
int IsDigit(char c);
int IsAddop(char c);

char GetName();
int GetNum();

void Emit(char *s);
void EmitLn(char *s);

void Init();
void InitTable();

char *NewLabel();
void PostLabel(char *label);
#endif
