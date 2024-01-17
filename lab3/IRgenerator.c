#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symboltable.h"

#include "IRgenerator.h"
extern FILE* fw;
int var_counter=0;
int tmp_counter=0;
int label_counter=0;
IR head;
IR tail=NULL;
int ir_counter=0;
void initialize_head()
{
    head=(IR)malloc(sizeof(struct InterCode));
    head->next=NULL;
    tail=head;
}
void generatingIR(TreeNode* root)
{
    if (!strcmp(root->tag,"ExtDef"))
    {
        translate_ExtDef(root);
        //printf("%p\n",head->next);
        return;
    }
    TreeNode* tmp=root->children_queue;
    while (tmp)
    {
        generatingIR(tmp);
        tmp=tmp->next;
    }
}
void translate_ExtDef(TreeNode* root)
{
    
    translate_FucDec(root->children_queue->next);
    translate_Compst(root->children_queue->next->next);
    
    // printf("\n");
    // fprintf(fw,"\n");
}
void translate_FucDec(TreeNode* root)
{
    ir_counter++;
    // printf("FUNCTION %s :\n",root->children_queue->text);
    // fprintf(fw,"FUNCTION %s :\n",root->children_queue->text);
    IR newnode=(IR)malloc(sizeof(struct InterCode));
    newnode->next=NULL;
    newnode->kind=FUNCTION_STATE;
    Operand f=(Operand)malloc(sizeof(struct Operand_));
    f->kind=FUNCTION;
    f->u.func_name=(char*)malloc(100);
    strcpy(f->u.func_name,root->children_queue->text);
    newnode->u.func.f=f;
    tail->next=newnode;
    tail=tail->next;
    //translate varlist
    if (strcmp(root->children_queue->next->next->tag,"RP"))
    {
        
        translate_VarList(root->children_queue->next->next);
    }
}

void translate_VarList(TreeNode* root)
{
    //translate ParamDec
    //printf("%s\n",root->tag);
    translate_ParamDec(root->children_queue);
    //printf("flag\n");
    if (root->children_queue->next)
    {
        translate_VarList(root->children_queue->next->next);
    }
}

void translate_ParamDec(TreeNode* root)
{
    var_counter++;
    TreeNode* child=root->children_queue;
    TreeNode* VarDec=child->next;
    //printf("%p\n",VarDec->children_queue->text);
    struct tableItem *tmp=search_id(VarDec->children_queue->text);
    tmp->other_name=(char*)malloc(5);
    sprintf(tmp->other_name,"v%d",var_counter);
    // printf("PARAM %s\n",tmp->other_name);
    // fprintf(fw,"PARAM %s\n",tmp->other_name);
    ir_counter++;
    IR newnode=(IR)malloc(sizeof(struct InterCode));
    newnode->next=NULL;
    newnode->kind=PARAM;
    Operand f=(Operand)malloc(sizeof(struct Operand_));
    f->kind=VARIABLE;
    f->u.name=(char*)malloc(5);
    strcpy(f->u.name,tmp->other_name);
    newnode->u.param.p=f;
    tail->next=newnode;
    tail=tail->next;
}
void translate_Compst(TreeNode* root)
{
    translate_DefList(root->children_queue->next);
    translate_StmtList(root->children_queue->next->next);
}
void translate_DefList(TreeNode* root)
{
    if (root->epi_flag)
    {
        return;
    }
    translate_Def(root->children_queue);
    translate_DefList(root->children_queue->next);
}

void translate_Def(TreeNode* root)
{
    translate_DecList(root->children_queue->next);
}
void translate_DecList(TreeNode* root)
{
    TreeNode* child=root->children_queue;
    translate_Dec(child);
    if (child->next)
    {
        translate_DecList(child->next->next);
    }
}
void translate_Dec(TreeNode* root)
{
    TreeNode* child=root->children_queue;
    if (child->next)
    {
        //Exp
        char* t=translate_Exp(child->next->next);
        char* on=translate_VarDec(child);
        // printf("%s := %s\n",on,t);
        // fprintf(fw,"%s := %s\n",on,t);
        ir_counter++;
        IR newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=ASSIGN;
        Operand right=(Operand)malloc(sizeof(struct Operand_));
        Operand left=(Operand)malloc(sizeof(struct Operand_));
        right->u.name=(char*)malloc(100);
        left->u.name=(char*)malloc(100);
        strcpy(right->u.name,t);
        strcpy(left->u.name,on);
        newnode->u.assign.left=left;
        newnode->u.assign.right=right;
        right->kind=(t[0]=='t')?TEMP:VARIABLE;
        left->kind=(on[0]=='t')?TEMP:VARIABLE;
        tail->next=newnode;
        tail=tail->next;
    }
    else
    {
        translate_VarDec(child);
    }
}
char* translate_VarDec(TreeNode* root)
{
    TreeNode* child=root->children_queue;

    while (strcmp(child->tag,"ID"))
    {
        child=child->children_queue;
    }
    if (root->children_queue->next)
    {
        // printf("DEC %s %d\n",child->text,4*root->children_queue->next->next->intvalue);
        // fprintf(fw,"DEC %s %d\n",child->text,4*root->children_queue->next->next->intvalue);
        ir_counter++;
        IR newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=DEC;
        Operand x=(Operand)malloc(sizeof(struct Operand_));
        Operand size=(Operand)malloc(sizeof(struct Operand_));
        x->u.name=(char*)malloc(100);
        strcpy(x->u.name,child->text);
        size->u.intvalue=4*root->children_queue->next->next->intvalue;
        newnode->u.dec.x=x;
        newnode->u.dec.size=size;
        x->kind=VARIABLE;
        size->kind=INT;
        tail->next=newnode;
        tail=tail->next;
    }
    var_counter++;
    struct tableItem *tmp=search_id(child->text);
    tmp->other_name=(char*)malloc(5);
    sprintf(tmp->other_name,"v%d",var_counter);
    return tmp->other_name;
}

char* translate_Exp(TreeNode* root)
{
    TreeNode* child=root->children_queue;
    if (!strcmp(child->tag,"ID"))
    {
        if (!child->next)
        {
            struct tableItem* tmp=search_id(child->text);
            return tmp->other_name;
        }
        else
        {
            if (!strcmp(child->text,"read"))
            {
                tmp_counter++;
                char* vt=(char*)malloc(5);
                // printf("READ t%d\n",tmp_counter);
                // fprintf(fw,"READ t%d\n",tmp_counter);
                sprintf(vt,"t%d",tmp_counter);
                ir_counter++;
                IR newnode=(IR)malloc(sizeof(struct InterCode));
                newnode->next=NULL;
                newnode->kind=READ;
                Operand t=(Operand)malloc(sizeof(struct Operand_));
                t->kind=TEMP;
                t->u.name=(char*)malloc(5);
                strcpy(t->u.name,vt);
                newnode->u.r.t=t;
                tail->next=newnode;
                tail=tail->next;
                return vt;
            }
            if (!strcmp(child->text,"write"))
            {
                int n=0;
                char* args_list[100];
                translate_Args(child->next->next,args_list,&n);
                // printf("WRITE %s\n",args_list[0]);
                // fprintf(fw,"WRITE %s\n",args_list[0]);
                ir_counter++;
                IR newnode=(IR)malloc(sizeof(struct InterCode));
                newnode->next=NULL;
                newnode->kind=WRITE;
                Operand t=(Operand)malloc(sizeof(struct Operand_));
                t->kind=(args_list[0][0]=='t')?TEMP:VARIABLE;
                t->u.name=(char*)malloc(5);
                strcpy(t->u.name,args_list[0]);
                newnode->u.w.t=t;
                tail->next=newnode;
                tail=tail->next;
                return NULL;
            }
            if (strcmp(child->next->next->tag,"RP"))
            {
                int n=0;
                char* args_list[100];
                translate_Args(child->next->next,args_list,&n);
                int i;
                for (i=n-1;i>=0;i--)
                {
                    // printf("ARG %s\n",args_list[i]);
                    // fprintf(fw,"ARG %s\n",args_list[i]);
                    ir_counter++;
                    IR newnode=(IR)malloc(sizeof(struct InterCode));
                    newnode->next=NULL;
                    newnode->kind=ARG;
                    Operand t=(Operand)malloc(sizeof(struct Operand_));
                    t->kind=(args_list[i][0]=='t')?TEMP:VARIABLE;
                    t->u.name=(char*)malloc(5);
                    strcpy(t->u.name,args_list[i]);
                    newnode->u.args.arg=t;
                    tail->next=newnode;
                    tail=tail->next;
                }
            }
            char* vt=(char*)malloc(5);
            tmp_counter++;
            // printf("t%d := CALL %s\n",tmp_counter,child->text);
            // fprintf(fw,"t%d := CALL %s\n",tmp_counter,child->text);
            sprintf(vt,"t%d",tmp_counter);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=CALL;
            Operand x=(Operand)malloc(sizeof(struct Operand_));
            Operand func=(Operand)malloc(sizeof(struct Operand_));
            x->kind=TEMP;
            func->kind=FUNCTION;
            x->u.name=(char*)malloc(5);
            strcpy(x->u.name,vt);
            func->u.func_name=(char*)malloc(100);
            strcpy(func->u.func_name,child->text);
            newnode->u.call.x=x;
            newnode->u.call.func=func;
            tail->next=newnode;
            tail=tail->next;
            return vt;
        }
    }
    if (!strcmp(child->tag,"INT"))
    {
        char* vt=(char*)malloc(5);
        tmp_counter++;
        // printf("t%d := #%d\n",tmp_counter,child->intvalue);
        // fprintf(fw,"t%d := #%d\n",tmp_counter,child->intvalue);
        sprintf(vt,"t%d",tmp_counter);
        ir_counter++;
        IR newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=ASSIGN;
        Operand right=(Operand)malloc(sizeof(struct Operand_));
        Operand left=(Operand)malloc(sizeof(struct Operand_));
        left->u.name=(char*)malloc(100);
        right->u.intvalue=child->intvalue;
        strcpy(left->u.name,vt);
        newnode->u.assign.left=left;
        newnode->u.assign.right=right;
        right->kind=INT;
        left->kind=TEMP;
        tail->next=newnode;
        tail=tail->next;
        return vt;
    }
    if (!strcmp(child->tag,"FLOAT"))
    {
        char* vt=(char*)malloc(5);
        tmp_counter++;
        // printf("t%d := #%d\n",tmp_counter,child->floatvalue);
        // fprintf(fw,"t%d := #%f\n",tmp_counter,child->floatvalue);
        sprintf(vt,"t%d",tmp_counter);
        ir_counter++;
        IR newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=ASSIGN;
        Operand right=(Operand)malloc(sizeof(struct Operand_));
        Operand left=(Operand)malloc(sizeof(struct Operand_));
        left->u.name=(char*)malloc(100);
        right->u.floatvalue=child->floatvalue;
        strcpy(left->u.name,vt);
        newnode->u.assign.left=left;
        newnode->u.assign.right=right;
        right->kind=FLOAT;
        left->kind=TEMP;
        tail->next=newnode;
        tail=tail->next;
        return vt;
    }
    if (!strcmp(child->tag,"NOT"))
    {
        //TODO
        child->next->t=root->f;
        child->next->f=root->t;
        translate_Exp(child->next);
        return NULL;
    }
    if (!strcmp(child->tag,"MINUS"))
    {
        char* t=translate_Exp(child->next);
        char* vt=(char*)malloc(5);
        tmp_counter++;
        // printf("t%d := #0 - %s\n",tmp_counter,t);
        // fprintf(fw,"t%d := #0 - %s\n",tmp_counter,t);
        sprintf(vt,"t%d",tmp_counter);
        ir_counter++;
        IR newcode=(IR)malloc(sizeof(struct InterCode));
        newcode->kind=MINUS;
        newcode->next=NULL;
        Operand res=(Operand)malloc(sizeof(struct Operand_));
        Operand op1=(Operand)malloc(sizeof(struct Operand_));
        Operand op2=(Operand)malloc(sizeof(struct Operand_));
        res->kind=TEMP;
        res->u.name=(char*)malloc(5);
        strcpy(res->u.name,vt);
        op1->kind=INT;
        op1->u.intvalue=0;
        op2->kind=(t[0]=='t')?TEMP:VARIABLE;
        op2->u.name=(char*)malloc(5);
        strcpy(op2->u.name,t);
        newcode->u.binop.result=res;
        newcode->u.binop.op1=op1;
        newcode->u.binop.op2=op2;
        tail->next=newcode;
        tail=tail->next;
        return vt;
    }
    if (!strcmp(child->tag,"LP"))
    {
        return translate_Exp(child->next);
    }
    if (!strcmp(child->tag,"Exp"))
    {
        if (!strcmp(child->next->tag,"RELOP"))
        {
            char* t1=translate_Exp(child);
            char* t2=translate_Exp(child->next->next);
            char* t=(char*)malloc(100);
            // printf("IF %s %s %s GOTO label%d\nGOTO label%d\n",t1,child->next->text,t2,root->t,root->f);
            // fprintf(fw,"IF %s %s %s GOTO label%d\nGOTO label%d\n",t1,child->next->text,t2,root->t,root->f);
            ir_counter++;
            IR newcode=(IR)malloc(sizeof(struct InterCode));
            newcode->kind=IFGOTO;
            newcode->next=NULL;
            Operand x1=(Operand)malloc(sizeof(struct Operand_));
            Operand relop=(Operand)malloc(sizeof(struct Operand_));
            Operand y=(Operand)malloc(sizeof(struct Operand_));
            Operand z=(Operand)malloc(sizeof(struct Operand_));
            x1->kind=(t1[0]=='t')?TEMP:VARIABLE;
            y->kind=(t2[0]=='t')?TEMP:VARIABLE;
            z->kind=LABEL;
            x1->u.name=(char*)malloc(5);
            y->u.name=(char*)malloc(5);
            relop->u.rel=(char*)malloc(5);
            strcpy(x1->u.name,t1);
            strcpy(y->u.name,t2);
            strcpy(relop->u.rel,child->next->text);
            z->u.label_no=root->t;
            newcode->u.jcond.x=x1;
            newcode->u.jcond.y=y;
            newcode->u.jcond.z=z;
            newcode->u.jcond.relop=relop;
            tail->next=newcode;
            tail=tail->next;
            ir_counter++;
            IR newcode1=(IR)malloc(sizeof(struct InterCode));
            newcode1->kind=GOTO;
            newcode1->next=NULL;
            Operand x2=(Operand)malloc(sizeof(struct Operand_));
            x2->kind=LABEL;
            x2->u.label_no=root->f;
            newcode1->u.jmp.x=x2;
            tail->next=newcode1;
            tail=tail->next;
            return NULL;

        }
        if (!strcmp(child->next->tag,"PLUS")||!strcmp(child->next->tag,"MINUS")||!strcmp(child->next->tag,"STAR")||!strcmp(child->next->tag,"DIV"))
        {
            char* t1=translate_Exp(child);
            char* t2=translate_Exp(child->next->next);
            char* vt=(char*)malloc(5);
            tmp_counter++;
            // printf("t%d := %s %s %s\n",tmp_counter,t1,child->next->text,t2);
            // fprintf(fw,"t%d := %s %s %s\n",tmp_counter,t1,child->next->text,t2);
            sprintf(vt,"t%d",tmp_counter);
            ir_counter++;
            IR newcode=(IR)malloc(sizeof(struct InterCode));
            if (!strcmp(child->next->tag,"PLUS"))
            {
                newcode->kind=PLUS;
            }
            else if (!strcmp(child->next->tag,"MINUS"))
            {
                newcode->kind=MINUS;
            }
            else if (!strcmp(child->next->tag,"STAR"))
            {
                newcode->kind=STAR;
            }
            else
            {
                newcode->kind=DIV;
            }
            newcode->next=NULL;
            Operand res=(Operand)malloc(sizeof(struct Operand_));
            Operand op1=(Operand)malloc(sizeof(struct Operand_));
            Operand op2=(Operand)malloc(sizeof(struct Operand_));
            res->kind=TEMP;
            res->u.name=(char*)malloc(5);
            strcpy(res->u.name,vt);
            op1->kind=(t1[0]=='t')?TEMP:VARIABLE;
            op1->u.name=(char*)malloc(5);
            op2->kind=(t2[0]=='t')?TEMP:VARIABLE;
            op2->u.name=(char*)malloc(5);
            op1->u.name=(char*)malloc(5);
            strcpy(op1->u.name,t1);
            strcpy(op2->u.name,t2);
            newcode->u.binop.result=res;
            newcode->u.binop.op1=op1;
            newcode->u.binop.op2=op2;
            tail->next=newcode;
            tail=tail->next;
            return vt;
        }
        if (!strcmp(child->next->tag,"AND"))
        {
            //related to jump,TODO
            label_counter++;
            child->t=label_counter;
            child->f=root->n;
            translate_Exp(child);
            // printf("LABEL label%d :\n",child->t);
            // fprintf(fw,"LABEL label%d :\n",child->t);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=LABEL_STATE;
            Operand l=(Operand)malloc(sizeof(struct Operand_));
            l->kind=LABEL;
            l->u.label_no=child->t;
            newnode->u.label.l=l;
            tail->next=newnode;
            tail=tail->next;
            child->next->next->t=root->t;
            child->next->next->f=root->f;
            translate_Exp(child->next->next);
            return NULL;
        }
        if (!strcmp(child->next->tag,"OR"))
        {
            //related to jump,TODO
            child->t=root->t;
            label_counter++;
            child->f=label_counter;
            translate_Exp(child);
            // printf("LABEL label%d :\n",child->f);
            // fprintf(fw,"LABEL label%d :\n",child->f);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=LABEL_STATE;
            Operand l=(Operand)malloc(sizeof(struct Operand_));
            l->kind=LABEL;
            l->u.label_no=child->f;
            newnode->u.label.l=l;
            tail->next=newnode;
            tail=tail->next;
            child->next->next->t=root->t;
            child->next->next->f=root->f;
            return NULL;
        }
        if (!strcmp(child->next->tag,"ASSIGNOP"))
        {
            if (!strcmp(child->children_queue->tag,"ID"))
            {
                struct tableItem* tmp=search_id(child->children_queue->text);
                char* t=translate_Exp(child->next->next);
                // printf("%s := %s\n",tmp->other_name,t);
                // fprintf(fw,"%s := %s\n",tmp->other_name,t);
                ir_counter++;
                IR newnode=(IR)malloc(sizeof(struct InterCode));
                newnode->next=NULL;
                newnode->kind=ASSIGN;
                Operand right=(Operand)malloc(sizeof(struct Operand_));
                Operand left=(Operand)malloc(sizeof(struct Operand_));
                right->u.name=(char*)malloc(100);
                left->u.name=(char*)malloc(100);
                strcpy(right->u.name,t);
                strcpy(left->u.name,tmp->other_name);
                newnode->u.assign.left=left;
                newnode->u.assign.right=right;
                right->kind=(t[0]=='t')?TEMP:VARIABLE;
                left->kind=(tmp->other_name[0]=='t')?TEMP:VARIABLE;
                tail->next=newnode;
                tail=tail->next;
                return NULL;
            }
            char* t1=translate_Exp(child);
            char* t2=translate_Exp(child->next->next);
            // printf("%s[%s] := %s\n",child->children_queue->children_queue->text,t1,t2);
            // fprintf(fw,"%s[%s] := %s\n",child->children_queue->children_queue->text,t1,t2);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=ARRAY_ASSIGN;
            Operand right=(Operand)malloc(sizeof(struct Operand_));
            Operand index=(Operand)malloc(sizeof(struct Operand_));
            Operand left=(Operand)malloc(sizeof(struct Operand_));
            right->u.name=(char*)malloc(100);
            left->u.name=(char*)malloc(100);
            index->u.name=(char*)malloc(100);
            strcpy(right->u.name,t2);
            strcpy(index->u.name,t1);
            strcpy(left->u.name,child->children_queue->children_queue->text);
            newnode->u.array_assign.left=left;
            newnode->u.array_assign.right=right;
            newnode->u.array_assign.index=index;
            right->kind=(t2[0]=='t')?TEMP:VARIABLE;
            index->kind=(t1[0]=='t')?TEMP:VARIABLE;
            left->kind=VARIABLE;
            tail->next=newnode;
            tail=tail->next;
            return NULL;
        }
        if (!strcmp(child->next->tag,"LB"))
        {
            //for no high dim array
            struct tableItem* ID=child->children_queue;
            char *t=translate_Exp(child->next->next);
            char* vt=(char*)malloc(5);
            tmp_counter++;
            // printf("t%d := #4*%s\n",tmp_counter,t);
            // fprintf(fw,"t%d := #4*%s\n",tmp_counter,t);
            sprintf(vt,"t%d",tmp_counter);
            ir_counter++;
            IR newcode=(IR)malloc(sizeof(struct InterCode));
            newcode->kind=STAR;
            newcode->next=NULL;
            Operand res=(Operand)malloc(sizeof(struct Operand_));
            Operand op1=(Operand)malloc(sizeof(struct Operand_));
            Operand op2=(Operand)malloc(sizeof(struct Operand_));
            res->kind=TEMP;
            res->u.name=(char*)malloc(5);
            strcpy(res->u.name,vt);
            op1->kind=INT;
            op1->u.intvalue=5;
            op2->kind=(t[0]=='t')?TEMP:VARIABLE;
            op2->u.name=(char*)malloc(5);
            strcpy(op2->u.name,t);
            newcode->u.binop.result=res;
            newcode->u.binop.op1=op1;
            newcode->u.binop.op2=op2;
            tail->next=newcode;
            tail=tail->next;
            return vt;
        }
        if (!strcmp(child->next->tag,"DOT"))
        {
            char* t1=translate_Exp(child);
            struct tableItem* t2=search_id(child->next->next->text);
            char* vt=(char*)malloc(5);
            tmp_counter++;
            // printf("t%d := %s.%s\n",tmp_counter,t1,t2->other_name);
            // fprintf(fw,"t%d := %s.%s\n",tmp_counter,t1,t2->other_name);
            sprintf(vt,"t%d",tmp_counter);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=DOT_ASSIGN;
            Operand right=(Operand)malloc(sizeof(struct Operand_));
            Operand field=(Operand)malloc(sizeof(struct Operand_));
            Operand left=(Operand)malloc(sizeof(struct Operand_));
            right->u.name=(char*)malloc(100);
            left->u.name=(char*)malloc(100);
            field->u.name=(char*)malloc(100);
            strcpy(right->u.name,t1);
            strcpy(field->u.name,t2->other_name);
            strcpy(left->u.name,vt);
            newnode->u.dot_assign.left=left;
            newnode->u.dot_assign.right=right;
            newnode->u.dot_assign.field=field;
            right->kind=(t1[0]=='t')?TEMP:VARIABLE;
            field->kind=(t2->other_name[0]=='t')?TEMP:VARIABLE;
            left->kind=TEMP;
            tail->next=newnode;
            tail=tail->next;
            return vt;
        }
    }
}

void translate_Args(TreeNode* root,char* args_list[],int *n)
{
    *n=*n+1;
    TreeNode* child=root->children_queue;
    if (child->next)
    {
        translate_Args(child->next->next,args_list,n);
    }
    char* t=translate_Exp(child);
    args_list[*n-1]=t;
}

void translate_StmtList(TreeNode* root)
{
    if (root->epi_flag)
    {
        return;
    }
    translate_Stmt(root->children_queue);
    translate_StmtList(root->children_queue->next);
}

void translate_Stmt(TreeNode* root)
{
    TreeNode* child=root->children_queue;
    //printf("%s\n",child->tag);
    if (!strcmp(child->tag,"RETURN"))
    {
        if (!strcmp(child->next->children_queue->tag,"INT")||!strcmp(child->next->children_queue->tag,"FLOAT"))
        {
            char *t=translate_Exp(child->next);
            tmp_counter++;
            char* vt=(char*)malloc(5);
            // printf("RETURN %s\n",t);
            // fprintf(fw,"RETURN %s\n",t);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=RETURN;
            Operand x=(Operand)malloc(sizeof(struct Operand_));
            x->kind=TEMP;
            x->u.name=(char*)malloc(5);
            strcpy(x->u.name,t);
            newnode->u.ret.x=x;
            tail->next=newnode;
            tail=tail->next;
            return;
        }
        char *t=translate_Exp(child->next);
        tmp_counter++;
        char* vt=(char*)malloc(5);
        // printf("t%d := %s\n",tmp_counter,t);
        // fprintf(fw,"t%d := %s\n",tmp_counter,t);
        sprintf(vt,"t%d",tmp_counter);
        ir_counter++;
        IR newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=ASSIGN;
        Operand right=(Operand)malloc(sizeof(struct Operand_));
        Operand left=(Operand)malloc(sizeof(struct Operand_));
        right->u.name=(char*)malloc(100);
        left->u.name=(char*)malloc(100);
        strcpy(right->u.name,t);
        strcpy(left->u.name,vt);
        newnode->u.assign.left=left;
        newnode->u.assign.right=right;
        right->kind=(t[0]=='t')?TEMP:VARIABLE;
        left->kind=(vt[0]=='t')?TEMP:VARIABLE;
        tail->next=newnode;
        tail=tail->next;
        // printf("RETURN t%d\n",tmp_counter);
        // fprintf(fw,"RETURN t%d\n",tmp_counter);
        ir_counter++;
        newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=RETURN;
        Operand x=(Operand)malloc(sizeof(struct Operand_));
        x->kind=TEMP;
        x->u.name=(char*)malloc(5);
        strcpy(x->u.name,t);
        newnode->u.ret.x=x;
        tail->next=newnode;
        tail=tail->next;
        return;
    }
    else if (!strcmp(child->tag,"Exp"))
    {
        translate_Exp(child);
    }
    else if (!strcmp(child->tag,"IF"))
    {
        if (!child->next->next->next->next->next)
        {
            TreeNode* cond=child->next->next;
            label_counter++;
            cond->t=label_counter;
            label_counter++;
            root->n=label_counter;
            cond->f=root->n;
            translate_Exp(cond);
            //TODO
            // printf("LABEL label%d :\n",cond->t);
            // fprintf(fw,"LABEL label%d :\n",cond->t);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=LABEL_STATE;
            Operand l=(Operand)malloc(sizeof(struct Operand_));
            l->kind=LABEL;
            l->u.label_no=cond->t;
            newnode->u.label.l=l;
            tail->next=newnode;
            tail=tail->next;
            cond->next->next->n=root->n;
            translate_Stmt(cond->next->next);
            if (!root->next||!root->next->epi_flag)
            {
                // printf("LABEL label%d :\n",root->n);
                // fprintf(fw,"LABEL label%d :\n",root->n);
                ir_counter++;
                IR newnode=(IR)malloc(sizeof(struct InterCode));
                newnode->next=NULL;
                newnode->kind=LABEL_STATE;
                Operand l=(Operand)malloc(sizeof(struct Operand_));
                l->kind=LABEL;
                l->u.label_no=root->n;
                newnode->u.label.l=l;
                tail->next=newnode;
                tail=tail->next;
            }
            else
            {
                label_counter--;
            }
        }
        else
        {
            TreeNode* cond=child->next->next;
            label_counter++;
            cond->t=label_counter;
            label_counter++;
            cond->f=label_counter;
            label_counter++;
            root->n=label_counter;
            translate_Exp(cond);
            // printf("LABEL label%d :\n",cond->t);
            // fprintf(fw,"LABEL label%d :\n",cond->t);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=LABEL_STATE;
            Operand l=(Operand)malloc(sizeof(struct Operand_));
            l->kind=LABEL;
            l->u.label_no=cond->t;
            newnode->u.label.l=l;
            tail->next=newnode;
            tail=tail->next;
            cond->next->next->n=root->n;
            translate_Stmt(cond->next->next);
            // printf("GOTO label%d\n",root->n);
            // fprintf(fw,"GOTO label%d\n",root->n);
            if (strcmp(cond->next->next->children_queue->tag,"RETURN"))
            {
                ir_counter++;
                IR newcode1=(IR)malloc(sizeof(struct InterCode));
                newcode1->kind=GOTO;
                newcode1->next=NULL;
                Operand x2=(Operand)malloc(sizeof(struct Operand_));
                x2->kind=LABEL;
                x2->u.label_no=root->n;
                newcode1->u.jmp.x=x2;
                tail->next=newcode1;
                tail=tail->next;
            }
            // printf("LABEL label%d :\n",cond->f);
            // fprintf(fw,"LABEL label%d :\n",cond->f);
            ir_counter++;
            newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=LABEL_STATE;
            l=(Operand)malloc(sizeof(struct Operand_));
            l->kind=LABEL;
            l->u.label_no=cond->f;
            newnode->u.label.l=l;
            tail->next=newnode;
            tail=tail->next;
            cond->next->next->next->next->n=root->n;
            translate_Stmt(cond->next->next->next->next);
            
            if (!root->next||!root->next->epi_flag)
            {
                // printf("LABEL label%d :\n",root->n);
                // fprintf(fw,"LABEL label%d :\n",root->n);
                ir_counter++;
                IR newnode=(IR)malloc(sizeof(struct InterCode));
                newnode->next=NULL;
                newnode->kind=LABEL_STATE;
                Operand l=(Operand)malloc(sizeof(struct Operand_));
                l->kind=LABEL;
                l->u.label_no=root->n;
                newnode->u.label.l=l;
                tail->next=newnode;
                tail=tail->next;
            }
            else
            {
                label_counter--;
            }
        }
    }
    else if (!strcmp(child->tag,"WHILE"))
    {
        label_counter++;
        root->begin=label_counter;
        // printf("LABEL label%d :\n",root->begin);
        // fprintf(fw,"LABEL label%d :\n",root->begin);
        ir_counter++;
        IR newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=LABEL_STATE;
        Operand l=(Operand)malloc(sizeof(struct Operand_));
        l->kind=LABEL;
        l->u.label_no=root->begin;
        newnode->u.label.l=l;
        tail->next=newnode;
        tail=tail->next;
        TreeNode* cond=child->next->next;
        label_counter++;
        cond->t=label_counter;
        label_counter++;
        root->n=label_counter;
        cond->f=root->n;
        translate_Exp(cond);
        // printf("LABEL label%d :\n",cond->t);
        // fprintf(fw,"LABEL label%d :\n",cond->t);
        ir_counter++;
        newnode=(IR)malloc(sizeof(struct InterCode));
        newnode->next=NULL;
        newnode->kind=LABEL_STATE;
        l=(Operand)malloc(sizeof(struct Operand_));
        l->kind=LABEL;
        l->u.label_no=cond->t;
        newnode->u.label.l=l;
        tail->next=newnode;
        tail=tail->next;
        cond->next->next->n=root->begin;
        translate_Stmt(cond->next->next);
        // printf("GOTO label%d\n",root->begin);
        // fprintf(fw,"GOTO label%d\n",root->begin);
        ir_counter++;
        IR newcode1=(IR)malloc(sizeof(struct InterCode));
        newcode1->kind=GOTO;
        newcode1->next=NULL;
        Operand x2=(Operand)malloc(sizeof(struct Operand_));
        x2->kind=LABEL;
        x2->u.label_no=root->begin;
        newcode1->u.jmp.x=x2;
        tail->next=newcode1;
        tail=tail->next;
        if (!root->next||!root->next->epi_flag)
        {
            // printf("LABEL label%d :\n",root->n);
            // fprintf(fw,"LABEL label%d :\n",root->n);
            ir_counter++;
            IR newnode=(IR)malloc(sizeof(struct InterCode));
            newnode->next=NULL;
            newnode->kind=LABEL_STATE;
            Operand l=(Operand)malloc(sizeof(struct Operand_));
            l->kind=LABEL;
            l->u.label_no=root->n;
            newnode->u.label.l=l;
            tail->next=newnode;
            tail=tail->next;
        }
        else
        {
            label_counter--;
        }
    }
    else
    {
        translate_Compst(child);
    }
}

void output_IR()
{
    IR cur=head;
    int i;
    printf("%d\n",ir_counter);
    for (i=0;i<ir_counter;i++)
    {
        cur=cur->next;
        // printf("%p\n",cur->next);
        // printf("%d\n",cur->kind);
        // printf("%d\n",i);
        if (cur->kind==RETURN)
        {
            printf("RETURN %s\n",cur->u.ret.x->u.name);
            fprintf(fw,"RETURN %s\n",cur->u.ret.x->u.name);
        }
        if (cur->kind==FUNCTION_STATE)
        {
            if (i)
            {
                printf("\n");
                fprintf(fw,"\n");
            }
            printf("FUNCTION %s :\n",cur->u.func.f->u.func_name);
            fprintf(fw,"FUNCTION %s :\n",cur->u.func.f->u.func_name);
        }
        if (cur->kind==LABEL_STATE)
        {
            printf("LABEL label%d :\n",cur->u.label.l->u.label_no);
            fprintf(fw,"LABEL label%d :\n",cur->u.label.l->u.label_no);
        }
        if (cur->kind==READ)
        {
            printf("READ %s\n",cur->u.r.t->u.name);
            fprintf(fw,"READ %s\n",cur->u.r.t->u.name);
        }
        if (cur->kind==WRITE)
        {
            printf("WRITE %s\n",cur->u.w.t->u.name);
            fprintf(fw,"WRITE %s\n",cur->u.w.t->u.name);
        }
        if (cur->kind==ASSIGN)
        {
            if (cur->u.assign.right->kind==INT)
            {
                printf("%s := #%d\n",cur->u.assign.left->u.name,cur->u.assign.right->u.intvalue);
                fprintf(fw,"%s := #%d\n",cur->u.assign.left->u.name,cur->u.assign.right->u.intvalue);
            }
            else if (cur->u.assign.right->kind==FLOAT)
            {
                printf("%s := #%f\n",cur->u.assign.left->u.name,cur->u.assign.right->u.floatvalue);
                fprintf(fw,"%s := #%f\n",cur->u.assign.left->u.name,cur->u.assign.right->u.floatvalue);
            }
            else
            {
                printf("%s := %s\n",cur->u.assign.left->u.name,cur->u.assign.right->u.name);
                fprintf(fw,"%s := %s\n",cur->u.assign.left->u.name,cur->u.assign.right->u.name);
            }
            
        }
        if (cur->kind==PLUS)
        {
            if (cur->u.binop.op1->kind==INT||cur->u.binop.op1->kind==FLOAT)
            {
                printf("%s := #%d + %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := #%d + %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
            }
            else
            {
                printf("%s := %s + %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := %s + %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
            }
        }
        if (cur->kind==MINUS)
        {
            if (cur->u.binop.op1->kind==INT||cur->u.binop.op1->kind==FLOAT)
            {
                printf("%s := #%d - %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := #%d - %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
            }
            else
            {
                printf("%s := %s - %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := %s - %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
            }
        }
        if (cur->kind==STAR)
        {
            if (cur->u.binop.op1->kind==INT||cur->u.binop.op1->kind==FLOAT)
            {
                printf("%s := #%d * %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := #%d * %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
            }
            else
            {
                printf("%s := %s * %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := %s * %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
            }
        }
        if (cur->kind==DIV)
        {
            if (cur->u.binop.op1->kind==INT||cur->u.binop.op1->kind==FLOAT)
            {
                printf("%s := #%d / %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := #%d + %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.intvalue,cur->u.binop.op2->u.name);
            }
            else
            {
                printf("%s := %s / %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
                fprintf(fw,"%s := %s / %s\n",cur->u.binop.result->u.name,cur->u.binop.op1->u.name,cur->u.binop.op2->u.name);
            }
        }
        if (cur->kind==IFGOTO)
        {
            printf("IF %s %s %s GOTO label%d\n",cur->u.jcond.x->u.name,cur->u.jcond.relop->u.rel,cur->u.jcond.y->u.name,cur->u.jcond.z->u.label_no);
            fprintf(fw,"IF %s %s %s GOTO label%d\n",cur->u.jcond.x->u.name,cur->u.jcond.relop->u.rel,cur->u.jcond.y->u.name,cur->u.jcond.z->u.label_no);
        }
        if (cur->kind==GOTO)
        {
            printf("GOTO label%d\n",cur->u.jmp.x->u.label_no);
            fprintf(fw,"GOTO label%d\n",cur->u.jmp.x->u.label_no);
        }
        if (cur->kind==PARAM)
        {
            printf("PARAM %s\n",cur->u.param.p->u.name);
            fprintf(fw,"PARAM %s\n",cur->u.param.p->u.name);
        }
        if (cur->kind==ARG)
        {
            printf("ARG %s\n",cur->u.args.arg->u.name);
            fprintf(fw,"ARG %s\n",cur->u.args.arg->u.name);
        }
        if (cur->kind==DEC)
        {
            printf("DEC %s %d\n",cur->u.dec.x->u.name,cur->u.dec.size->u.intvalue);
            fprintf(fw,"DEC %s %d\n",cur->u.dec.x->u.name,cur->u.dec.size->u.intvalue);
        }
        if (cur->kind==CALL)
        {
            printf("%s := CALL %s\n",cur->u.call.x->u.name,cur->u.call.func->u.func_name);
            fprintf(fw,"%s := CALL %s\n",cur->u.call.x->u.name,cur->u.call.func->u.func_name);
        }
        if (cur->kind==ARRAY_ASSIGN)
        {
            printf("%s[%s] := %s\n",cur->u.array_assign.left->u.name,cur->u.array_assign.index->u.name,cur->u.array_assign.right->u.name);
            fprintf(fw,"%s[%s] := %s\n",cur->u.array_assign.left->u.name,cur->u.array_assign.index->u.name,cur->u.array_assign.right->u.name);
        }
        if (cur->kind==DOT_ASSIGN)
        {
            printf("%s[%s] := %s\n",cur->u.dot_assign.left->u.name,cur->u.dot_assign.right->u.name,cur->u.dot_assign.field->u.name);
            fprintf(fw,"%s[%s] := %s\n",cur->u.dot_assign.left->u.name,cur->u.dot_assign.right->u.name,cur->u.dot_assign.field->u.name);
        }
    }
}