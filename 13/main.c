#include <stdio.h>
#include <string.h>

#include "cradle.h"


void Expression();
void AssignOrProc();
void Assignment(char name);
void DoBlock();
void BeginBlock();
void Alloc(char name);
void Decl(void);
void TopDecls(void);
void DoProc(void);
void DoMain(void);
void Return();
void CallProc(char name);
void Call(char name);
void FormalList();
void FormalParam();
void Param();
int ParamList();
void LocDecl();
int LocDecls();

void Header();
void Prolog();
void Epilog();

void ProcProlog(char name, int num_local_params);
void ProcEpilog();


/* parse and tranlate an expression
 * vestigial version */
void Expression()
{
    char name = GetName();
    if (IsParam(name)) {
        LoadParam(ParamNumber(name));
    } else {
        LoadVar(name);
    }
}

/* decide if a statement is an assignment or procedure call */
void AssignOrProc()
{
    char name = GetName();
    char tmp_buf[MAX_BUF];
    switch (TypeOf(name)) {
        case ' ':
            Undefined(name);
            break;
        case 'v':
        case 'f':
            Assignment(name);
            break;
        case 'p':
            CallProc(name);
            break;
        default:
            sprintf(tmp_buf, "Identifier %c cannot be used here", name);
            Abort(tmp_buf);
            break;
    }
}

/* parse and tranlate an assignment statement */
void Assignment(char name)
{
    Match('=');
    Expression();
    if (IsParam(name)) {
        StoreParam(ParamNumber(name));
    } else {
        StoreVar(name);
    }
}

/* parse and translate a block of statement */
void DoBlock()
{
    while(strchr("e", Look) == NULL) {
        AssignOrProc();
        Fin();
    }
}

void CallProc(char name)
{
    int bytes_pushed = ParamList();
    Call(name);
    CleanStack(bytes_pushed);
}

/* call a procedure */
void Call(char name)
{
    char tmp_buf[MAX_BUF];
    sprintf(tmp_buf, "call %c", name);
    EmitLn(tmp_buf);
}

/* parse and translate a Begin-Block */
void BeginBlock()
{
    Match('b');
    Fin();
    DoBlock();
    Match('e');
    Fin();
}

/* allocate storage for a variable */
void Alloc(char name)
{
    if (InTable(name)) {
        Duplicate(name);
    }
    ST[name-'A'] = 'v';
    printf("\t%c : .int 0\n", name);
}

/* parse and translate a data declaration */
void Decl(void)
{
    printf(".section .data\n");
    Match('v');
    Alloc(GetName());
}

/* parse and translate global declarations */
void TopDecls(void)
{
    char tmp_buf[MAX_BUF];
    while(Look != '.') {
        switch(Look) {
            case 'v':
                Decl();
                break;
            case 'p':
                DoProc();
                break;
            case 'P':
                DoMain();
                break;
            default:
                sprintf(tmp_buf, "Unrecognized keyword %c", Look);
                Abort(tmp_buf);
                break;
        }
        Fin();
    }
}

void Header()
{
    printf(".global _start\n");
}

void Prolog()
{
    EmitLn(".section .text");
    EmitLn("_start:");
}

void Epilog()
{
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
}

void DoProc(void)
{
    Match('p');
    char name = GetName();
    Fin();
    if (InTable(name)) {
        Duplicate(name);
    }
    ST[name-'A'] = 'p';
    FormalList();
    int num_local_params = LocDecls();
    ProcProlog(name, num_local_params);
    BeginBlock();
    ProcEpilog();
    ClearParams();
}

void Return()
{
    EmitLn("ret");
}

/* parse and translate a main program 
 * <main program> ::= PROGRAM <ident> <begin-block>
 * */
void DoMain(void)
{
    Match('P');
    char name = GetName();
    Fin();
    if (InTable(name)) {
        Duplicate(name);
    }
    Prolog();
    BeginBlock();
}

/* process the formal parameter list of a procedure */
void FormalList()
{
    Match('(');
    if (Look != ')') {
        FormalParam();
        while(Look == ',') {
            Match(',');
            FormalParam();
        }
    }
    Match(')');
    Fin();
    Base = NumParams;
    NumParams = NumParams + 1;
}

/* process a formal parameter */
void FormalParam()
{
    AddParam(GetName());
}

/* process an actual parameter */
void Param()
{
    Expression();
    Push();
}

/* process the parameter list for a procedure call */
int ParamList()
{
    int num_params = 0;
    Match('(');
    if (Look != ')') {
        Param();
        num_params++;
        while(Look == ',') {
            Match(',');
            Param();
            num_params++;
        }
    }
    Match(')');
    return 4*num_params;
}

/* write the prolog for a procedure */
void ProcProlog(char name, int num_local_params)
{
    char tmp_buf[MAX_BUF];
    PostLabel(name);
    EmitLn("pushl %ebp");
    EmitLn("movl %esp, %ebp");
    sprintf(tmp_buf, "subl $%d, %%esp", 4*num_local_params);
    EmitLn(tmp_buf);
}

/* write the epilog for a procedure */
void ProcEpilog()
{
    EmitLn("movl %ebp, %esp");
    EmitLn("popl %ebp");
    EmitLn("ret");
}

/* parse and translate a local data declaration */
void LocDecl()
{
    Match('v');
    AddParam(GetName());
    Fin();
}

/* parse and translate local declarations */
int LocDecls()
{
    int num_params = 0;
    while(Look == 'v') {
        LocDecl();
        num_params ++;
    }
    return num_params;
}

int main(int argc, char *argv[])
{
    Init();
    Header();
    TopDecls();
    Epilog();
    return 0;
}
