#include "Tree.h"
typedef struct Operand_* Operand;
struct Operand_ {
    enum { VARIABLE, INT,FLOAT, TEMP,RELOP,FUNCTION,LABEL} kind;
    union {
        char* name;
        int intvalue;
        float floatvalue;
        char* rel;
        char *func_name;
        int label_no;
    } u;
};
typedef struct InterCode* IR;
struct InterCode
{
    enum { RETURN,FUNCTION_STATE,LABEL_STATE,READ,WRITE,ASSIGN, PLUS, MINUS, STAR, DIV ,IFGOTO,GOTO,PARAM,ARG,DEC,CALL,ARRAY_ASSIGN,DOT_ASSIGN} kind;
    union {
    struct { Operand right, left; } assign;
    struct { Operand result, op1, op2; } binop;
    struct {Operand t;} r;
    struct {Operand t;} w;
    struct {Operand x,relop,y,z;} jcond;
    struct {Operand x;} jmp;
    struct {Operand x,size;} dec;
    struct {Operand p;} param;
    struct {Operand arg;} args;
    struct {Operand f;} func;
    struct {Operand x,func;} call;
    struct {Operand l;} label;
    struct {Operand left,index,right;} array_assign;
    struct {Operand left,right,field;} dot_assign;
    struct {Operand x;} ret;
    } u;
    struct InterCode* next;
};


void generatingIR(TreeNode* root);

void translate_ExtDef(TreeNode* root);

void translate_FucDec(TreeNode* root);

void translate_VarList(TreeNode* root);

void translate_ParamDec(TreeNode* root);

void translate_Compst(TreeNode* root);

void translate_DefList(TreeNode* root);

void translate_Def(TreeNode* root);

void translate_DecList(TreeNode* root);

void translate_Dec(TreeNode* root);

char* translate_VarDec(TreeNode* root);

char* translate_Exp(TreeNode* root);

void translate_Args(TreeNode* root,char* args_list[],int *n);

void translate_StmtList(TreeNode* root);

void translate_Stmt(TreeNode* root);

void output_IR();

void initialize_head();