#ifndef _CRADLE_H
#define _CRADLE_H

#define MAX_BUF 100
char tmp[MAX_BUF];
char token_buf[MAX_BUF];

char Look;

void GetChar();

void Error(char *s);
void Abort(char *s);
void Expected(char *s);
void Match(char x);

int IsAlpha(char c);
int IsDigit(char c);
int IsAddop(char c);
int IsAlNum(char c);
int IsWhite(char c);

char *GetName();
char *GetNum();

void SkipWhite();

void Emit(char *s);
void EmitLn(char *s);

void Init();

#endif
