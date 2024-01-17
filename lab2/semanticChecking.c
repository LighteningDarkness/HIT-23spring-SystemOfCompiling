#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symboltable.h"
#include "semanticChecking.h"


extern int semantic_error;
extern FILE *fw;
//because program only produce extdeflist, search recursively.
void semantic_preorder(TreeNode* root,struct tableItem table[])
{
    TreeNode* tmp=root->children_queue;
    if (!strcmp(root->tag,"ExtDef"))
    {
        ExtDef(root);
    }
    //TODO
    while (tmp)
    {
        semantic_preorder(tmp,table);
        tmp=tmp->next;
    }
}



void ExtDecList(TreeNode* root,char* type)
{
    TreeNode* VarDec=root->children_queue;
    struct tableItem* newitem=(struct tableItem*)malloc(sizeof(struct tableItem));
    TreeNode* tmp;
    struct array* t=(struct array*)malloc(sizeof(struct array));
    newitem->list=NULL;
    newitem->next=NULL;
    newitem->ID=NULL;
    if (!strcmp(VarDec->children_queue->tag,"ID"))
    {
        newitem->isArray=0;
        newitem->ID=(char*)malloc(strlen(VarDec->children_queue->text));
        strcpy(newitem->ID,VarDec->children_queue->text);
        newitem->type=(char*)malloc(strlen(type));
        strcpy(newitem->type,type);
        newitem->lineno=root->linenumber;
        addItem(newitem);
    }
    else
    {
        newitem->isArray=1;
        newitem->type=(char*)malloc(strlen(type));
        strcpy(newitem->type,type);
        tmp=VarDec->children_queue;
        newitem->array_shape=t;
        t->dim=tmp->next->next->intvalue;
        t->next=NULL;
        tmp=tmp->children_queue;
        while (strcmp(tmp->tag,"ID"))
        {
            t->next=(struct array*)malloc(sizeof(struct array));
            t=t->next;
            t->next=NULL;
            t->dim=tmp->next->next->intvalue;
            tmp=tmp->children_queue;
        }
        newitem->ID=(char*)malloc(strlen(tmp->text));
        strcpy(newitem->ID,tmp->text);
        newitem->lineno=root->linenumber;
        addItem(newitem);
    }
    if (VarDec->next)
    {
        ExtDecList(VarDec->next->next,type);
    }
}


//function param
struct tableItem* ParamDec(TreeNode* root)
{
    struct tableItem* newvar=(struct tableItem*)malloc(sizeof(struct tableItem));
    TreeNode* Specifier=root->children_queue;
    TreeNode* VarDec=root->children_queue->next;
    TreeNode* tmp;
    struct array* t=(struct array*)malloc(sizeof(struct array));
    char *type;
    type=(char*)malloc(50);
    newvar->isFunc=0;
    newvar->next=NULL;
    int flag=0;
    if (!strcmp(Specifier->children_queue->tag,"TYPE"))
    {
        newvar->isStruct=0;
        strcpy(type,Specifier->children_queue->text);
    }
    else
    {
        newvar->isStruct=1;
        strcpy(type,"struct");
        TreeNode* opttag=Specifier->children_queue->children_queue->next;
        if (!opttag->epi_flag)
        {
            strcat(type,opttag->children_queue->text);
        }
        newvar->lineno=root->linenumber;
        if (opttag->next)
        {
            newvar->struct_list=DefList(opttag->next->next,type);
            struct_field_check(newvar->struct_list);
        }
        else
        {
            newvar->struct_list=search_struct_type(type,&flag);
            if (!flag)
            {
                semantic_error=1;
                printf("Error type 17 at Line %d: undefined struct: \"%s\".\n",newvar->lineno,type);
                fprintf(fw,"Error type 17 at Line %d: undefined struct: \"%s\".\n",newvar->lineno,type);
            }
        }

    }
    if (!strcmp(VarDec->children_queue->tag,"ID"))
    {
        newvar->isArray=0;
        newvar->ID=(char*)malloc(strlen(VarDec->children_queue->text));
        strcpy(newvar->ID,VarDec->children_queue->text);
        newvar->type=(char*)malloc(strlen(type));
        strcpy(newvar->type,type);
        newvar->lineno=root->linenumber;
        addItem(newvar);
        return newvar;
    }
    else
    {
        newvar->isArray=1;
        strcpy(newvar->type,type);
        tmp=VarDec->children_queue;
        newvar->array_shape=t;
        t->dim=tmp->next->next->intvalue;
        t->next=NULL;
        tmp=tmp->children_queue;
        while (strcmp(tmp->tag,"ID"))
        {
            t->next=(struct array*)malloc(sizeof(struct array));
            t=t->next;
            t->next=NULL;
            t->dim=tmp->next->next->intvalue;
            tmp=tmp->children_queue;
        }
        newvar->ID=(char*)malloc(strlen(tmp->text));
        strcpy(newvar->ID,tmp->text);
        newvar->lineno=root->linenumber;
        addItem(newvar);
        return newvar;
    }
}
struct tableItem* Dec(TreeNode* root,char* type,char* name)
{
    // printf("flag");
    TreeNode* VarDec=root->children_queue;
    struct tableItem* newvar=(struct tableItem*)malloc(sizeof(struct tableItem));
    TreeNode* tmp;
    struct array* t=(struct array*)malloc(sizeof(struct array));
    newvar->isFunc=0;
    newvar->next=NULL;
    if (name)
    {
        newvar->struct_name=(char*)malloc(strlen(name));
        strcpy(newvar->struct_name,name);
    }
    else
    {
        newvar->struct_name=NULL;
    }
    if (!strcmp(VarDec->children_queue->tag,"ID"))
    {
        newvar->isArray=0;
        newvar->ID=(char*)malloc(strlen(VarDec->children_queue->text));
        strcpy(newvar->ID,VarDec->children_queue->text);
        //printf("%s\n",newvar->ID);
        newvar->type=(char*)malloc(strlen(type));
        strcpy(newvar->type,type);
    }
    else
    {
        //printf("flag");
        newvar->isArray=1;
        newvar->type=(char*)malloc(strlen(type));
        strcpy(newvar->type,type);
        tmp=VarDec->children_queue;
        newvar->array_shape=t;
        t->dim=tmp->next->next->intvalue;
        t->next=NULL;
        tmp=tmp->children_queue;
        while (strcmp(tmp->tag,"ID"))
        {
            t->next=(struct array*)malloc(sizeof(struct array));
            t=t->next;
            t->next=NULL;
            t->dim=tmp->next->next->intvalue;
            tmp=tmp->children_queue;
        }
        //printf("%p\n",newvar->array_shape);
        newvar->ID=(char*)malloc(strlen(tmp->text));
        strcpy(newvar->ID,tmp->text);
    }
    if (VarDec->next)
    {
        Exp(VarDec->next->next);
    }
    newvar->lineno=root->linenumber;
    if (type[0]=='s')
    {
        //printf("flag\n");
        int flag=0;
        //printf("%s\n",type);
        struct tableItem* member=search_struct_type(type,&flag);
        // printf("%s\n",newvar->ID);
        // printf("%s\n",member->next->ID);
        if (!flag)
        {
            semantic_error=1;
            printf("Error type 17 at Line %d: undefined struct: \"%s\".\n",root->linenumber,type);
            fprintf(fw,"Error type 17 at Line %d: undefined struct: \"%s\".\n",root->linenumber,type);
        }
        newvar->isStruct=1;
        newvar->struct_list=member;
        //printf("flag:%p\n",newvar->struct_list);
        addItem(newvar);
    }
    else
    {
        newvar->isStruct=0;
        addItem(newvar);
    }
    
    return newvar;
}

struct tableItem* DecList(TreeNode* root,char* type,char* name)
{
    
    struct tableItem* newvar=Dec(root->children_queue,type,name);
    //printf("%s\n",newvar->ID);
    newvar->next=NULL;
    if (root->children_queue->next)
    {
        newvar->next=DecList(root->children_queue->next->next,type,name);
    }
    return newvar;
}
struct tableItem* DefList(TreeNode* root,char* name)
{
    if (root->epi_flag)
    {
        return NULL;
    }
    struct tableItem* newvar=Def(root->children_queue,name);
    struct tableItem* tmp=newvar;
    while (tmp->next)
    {
        tmp=tmp->next;
    }
    tmp->next=DefList(root->children_queue->next,name);
    //struct tableItem* tmp=newvar;
    //check struct variables
    // while (tmp)
    // {
    //     struct tableItem* iter=tmp->next;
    //     while (iter)
    //     {
    //         if (!strcmp(tmp->ID,iter->ID))
    //         {
    //             semantic_error=1;
    //             printf("Error type 15 at Line %d: redefined field: %s.\n",iter->lineno,iter->ID);
    //             fprintf(fw,"Error type 15 at Line %d: redefined field: %s.\n",iter->lineno,iter->ID);
    //         }
    //         iter=iter->next;
    //     }
    //     tmp=tmp->next;
    // }
    //printf("%s\n",newvar->next->ID);
    return newvar;
}
struct tableItem* Def(TreeNode* root,char* name)
{
    TreeNode* Specifier=root->children_queue;
    char *type;
    type=(char*)malloc(50);
    struct tableItem* member=NULL;
    int flag=0;
    struct tableItem* res;
    if (!strcmp(Specifier->children_queue->tag,"TYPE"))
    {
        strcpy(type,Specifier->children_queue->text);
    }
    else
    {
        strcpy(type,"struct");
        TreeNode* opttag=Specifier->children_queue->children_queue->next;
        if (!opttag->epi_flag)
        {
            strcat(type,opttag->children_queue->text);
            //printf("%s\n",type);
        }
        if (opttag->next)
        {
            //TODO
            member=DefList(opttag->next->next,type);
        }
        // else
        // {
        //     member=search_struct_type(type,&flag);
        //     if (!flag)
        //     {
        //         semantic_error=1;
        //         printf("Error type 17 at Line %d: undefined struct: \"%s\".\n",root->linenumber,type);
        //         fprintf(fw,"Error type 17 at Line %d: undefined struct: \"%s\".\n",root->linenumber,type);
        //     }
        // }
    }
    res=DecList(root->children_queue->next,type,name);
    //printf("%s\n",res->next->ID);
    //sth wrong
    //res->struct_list=member;
    //printf("%p\n",member);
    res->lineno=root->linenumber;
    return res;

}



struct tableItem* VarList(TreeNode* root)
{
    TreeNode* paramDec=root->children_queue;
    struct tableItem* res=ParamDec(paramDec);
    if (paramDec->next)
    {
        res->next=VarList(paramDec->next->next);
    }

    return res;
}

void ExtDef(TreeNode* root)
{
    
    TreeNode* Specifier=root->children_queue;
    char *type;
    type=(char*)malloc(50);
    struct tableItem* newitem=(struct tableItem*)malloc(sizeof(struct tableItem));
    newitem->list=NULL;
    newitem->next=NULL;
    newitem->ID=NULL;
    newitem->struct_list=NULL;
    newitem->lineno=root->linenumber;
    int flag=0;
    if (!strcmp(Specifier->children_queue->tag,"TYPE"))
    {
        newitem->isStruct=0;
        //here
        
        strcpy(type,Specifier->children_queue->text);
    }
    else
    {
        newitem->isStruct=1;
        strcpy(type,"struct");
        TreeNode* opttag=Specifier->children_queue->children_queue->next;
        if (!opttag->epi_flag)
        {
            strcat(type,opttag->children_queue->text);
        }
        newitem->lineno=root->linenumber;
        if (opttag->next)
        {
            newitem->struct_list=DefList(opttag->next->next,type);
            //printf("%p\n",newitem->struct_list);
            struct_field_check(newitem->struct_list);
        }
        else
        {
            newitem->struct_list=search_struct_type(type,&flag);
            
            if (!flag)
            {
                semantic_error=1;
                printf("Error type 17 at Line %d: undefined struct: \"%s\".\n",newitem->lineno,type);
                fprintf(fw,"Error type 17 at Line %d: undefined struct: \"%s\".\n",newitem->lineno,type);
            }
        }

    }

    if (!strcmp(Specifier->next->tag,"SEMI"))
    {
        newitem->type=(char*)malloc(strlen(type));
        newitem->lineno=root->linenumber;
        if (newitem->isStruct)
        {
            int i;
            int flag=0;
            for (i=0;i<len_struct_type;i++)
            {
                if (!strcmp(type,struct_type[i]))
                {
                    semantic_error=1;
                    printf("Error type 16 at Line %d: Duplicated struct name.\n",newitem->lineno);
                    fprintf(fw,"Error type 16 at Line %d: Duplicated struct name.\n",newitem->lineno);
                    flag=1;
                    break;
                }
            }
            if (!flag)
            {
                
                struct_type[len_struct_type]=type;
                len_struct_type++;
            }
        }
        strcpy(newitem->type,type);
        
        //printf("%s\n",newitem->type);
        //printf("%s\n",newitem->struct_list->ID);

        addItem(newitem);
    }
    if (!strcmp(Specifier->next->tag,"ExtDecList"))
    {
        ExtDecList(Specifier->next,type);
    }
    if (!strcmp(Specifier->next->tag,"FunDec"))
    {
        newitem->isFunc=1;
        newitem->type=(char*)malloc(strlen(type));
        strcpy(newitem->type,type);
        newitem->ID=(char*)malloc(strlen(Specifier->next->children_queue->text));
        strcpy(newitem->ID,Specifier->next->children_queue->text);
        if (!strcmp(Specifier->next->children_queue->next->next->tag,"VarList"))
        {
            newitem->list=VarList(Specifier->next->children_queue->next->next);
        }
        // struct tableItem* tmp=newitem->list;
        // while (tmp)
        // {
        //     struct tableItem* iter=tmp->next;
        //     while (iter)
        //     {
        //         if (!strcmp(tmp->ID,iter->ID))
        //         {
        //             semantic_error=1;
        //             fprintf(fw,"Error type 3 at Line %d: redefined varibale: %s.\n",iter->lineno,iter->ID);
        //         }
        //         iter=iter->next;
        //     }
        //     tmp=tmp->next;
        // }
        addItem(newitem);
        //TODO
        TreeNode* CompSt=Specifier->next->next;
        //printf("%d\n",CompSt->children_queue->next->epi_flag);
        DefList(CompSt->children_queue->next,NULL);
        StmtList(CompSt->children_queue->next->next,type);
    }
}

void struct_field_check(struct tableItem* list)
{
    struct tableItem* tmp=list;
    while (tmp)
    {
        struct tableItem* iter=tmp->next;
        while (iter)
        {
            if (!strcmp(tmp->ID,iter->ID))
            {
                semantic_error=1;
                printf("Error type 15 at Line %d: redefined field: %s.\n",iter->lineno,iter->ID);
                fprintf(fw,"Error type 15 at Line %d: redefined field: %s.\n",iter->lineno,iter->ID);
            }
            iter=iter->next;
        }
        tmp=tmp->next;
    }
}
//all kinds of exp, return the type of the result(if any)
char* Exp(TreeNode* root)
{
    TreeNode* child=root->children_queue;
    if (!strcmp(child->tag,"ID"))
    {
        struct tableItem* res=search_id(child->text);
        if (!res&&!child->next)
        {
            semantic_error=1;
            printf("Error type 1 at Line %d: undefined variable: %s.\n",root->linenumber,child->text);
            fprintf(fw,"Error type 1 at Line %d: undefined varibale: %s.\n",root->linenumber,child->text);
            return "none";
        }
        if (!res&&child->next)
        {
            semantic_error=1;
            printf("Error type 2 at Line %d: undefined function: %s.\n",root->linenumber,child->text);
            fprintf(fw,"Error type 2 at Line %d: undefined varibale: %s.\n",root->linenumber,child->text);
            return "none";
        }
        if (res&&child->next)
        {
            if (!res->isFunc)
            {
                semantic_error=1;
                printf("Error type 11 at Line %d: not a function.\n",root->linenumber);
                fprintf(fw,"Error type 11 at Line %d: not a function.\n",root->linenumber);
                return "not a func";
            }
            if (!strcmp(child->next->next->tag,"RP"))
            {
                if (res->list)
                {
                    semantic_error=1;
                    printf("Error type 9 at Line %d: unsupported args.\n",root->linenumber);
                    fprintf(fw,"Error type 9 at Line %d: unsupported args.\n",root->linenumber);
                }
                return res->type;
            }
            else
            {
                char* argslist[100];
                int j;
                for (j=0;j<100;j++)
                {
                    argslist[j]=NULL;
                }
                Args(child->next->next,argslist,0);
                struct tableItem* l1=res->list;
                int len=0;
                while (l1)
                {
                    len++;
                    l1=l1->next;
                }
                int i;
                l1=res->list;
                for (i=0;i<len;i++)
                {
                    if (strcmp(l1->type,argslist[i])||!argslist[i])
                    {
                        semantic_error=1;
                        printf("Error type 9 at Line %d: unsupported args.\n",root->linenumber);
                        fprintf(fw,"Error type 9 at Line %d: unsupported args: %s.\n",root->linenumber);
                        break;
                    }
                }
                if (argslist[len])
                {
                    semantic_error=1;
                    printf("Error type 9 at Line %d: unsupported args.\n",root->linenumber);
                    fprintf(fw,"Error type 9 at Line %d: unsupported args.\n",root->linenumber);
                }
                return res->type;
            }

        }
        return res->type;
    }
    if (!strcmp(child->tag,"INT"))
    {
        return "int";
    }
    if (!strcmp(child->tag,"FLOAT"))
    {
        return "float";
    }
    if (!strcmp(child->tag,"NOT")||!strcmp(child->tag,"LP")||!strcmp(child->tag,"MINUS"))
    {
        if (!strcmp(child->tag,"NOT")&&strcmp(Exp(child->next),"INT"))
        {
            semantic_error=1;
            printf("Error type 7 at Line %d:  Type mismatched for operands.\n",child->linenumber);
            fprintf(fw,"Error type 7 at Line %d:  Type mismatched for operands.\n",child->linenumber);
        }
        if (!strcmp(child->tag,"MINUS")&&(strcmp(Exp(child->next),"INT")&&strcmp(Exp(child->next),"FLOAT")))
        {
            semantic_error=1;
            printf("Error type 7 at Line %d:  Type mismatched for operands.\n",child->linenumber);
            fprintf(fw,"Error type 7 at Line %d:  Type mismatched for operands.\n",child->linenumber);
        }
        return Exp(child->next);
    }
    if (!strcmp(child->tag,"Exp"))
    {
        if (!strcmp(child->next->tag,"RELOP")||!strcmp(child->next->tag,"PLUS")||!strcmp(child->next->tag,"MINUS")||!strcmp(child->next->tag,"STAR")||!strcmp(child->next->tag,"DIV"))
        {
            // printf("%s\n",child->children_queue->next->tag);
            //printf("%s\n",Exp(child->next->next));
            // printf("%s\n",Exp(child->next->next));
            char* s1=Exp(child);
            char* s2=Exp(child->next->next);
            
            if (strcmp(s1,"none")&&strcmp(s2,"none")&&strcmp(s1,s2))
            {
                semantic_error=1;
                printf("Error type 7 at Line %d:  Type mismatched for operands.\n",child->next->linenumber);
                fprintf(fw,"Error type 7 at Line %d:  Type mismatched for operands.\n",child->next->linenumber);
            }
            //printf("%s\n",s1);
            if (!strcmp(child->next->tag,"RELOP"))
            {
                return "int";
            }
            return s1;
        }
        if (!strcmp(child->next->tag,"AND")||!strcmp(child->next->tag,"OR"))
        {
            if (strcmp(Exp(child),"INT")&&strcmp(Exp(child->next->next),"INT"))
            {
                semantic_error=1;
                printf("Error type 7 at Line %d:  Type mismatched for operands.\n",child->next->linenumber);
                fprintf(fw,"Error type 7 at Line %d:  Type mismatched for operands.\n",child->next->linenumber);
            }
            return Exp(child);
        }
        if (!strcmp(child->next->tag,"ASSIGNOP"))
        {
            char* s1=Exp(child);
            char* s2=Exp(child->next->next);
            if (strcmp(s1,"none")&&strcmp(s2,"none")&&strcmp(s1,s2))
            {
                semantic_error=1;
                printf("Error type 5 at Line %d:  Type mismatched for assignment.\n",child->next->linenumber);
                fprintf(fw,"Error type 5 at Line %d:  Type mismatched for assignment.\n",child->next->linenumber);
            }
            if (strcmp(child->children_queue->tag,"ID")&&(!child->children_queue->next||strcmp(child->children_queue->next->tag,"LB"))&&(!child->children_queue->next||strcmp(child->children_queue->next->tag,"DOT")))
            {
                semantic_error=1;
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",child->next->linenumber);
                fprintf(fw,"Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",child->next->linenumber);
            }
            return "none";
        }
        if (!strcmp(child->next->tag,"LB"))
        {
            TreeNode* tmp=child;
            int depth=0;
            struct tableItem* r;
            while (!strcmp(tmp->tag,"Exp")||!strcmp(tmp->tag,"LP"))
            {
                
                if (!strcmp(tmp->tag,"Exp"))
                {
                    if (strcmp(tmp->next->tag,"LB"))
                    {
                        semantic_error=1;
                        printf("Error type 10 at Line %d:  not an array.\n",child->next->linenumber);
                        fprintf(fw,"Error type 10 at Line %d:  not an array..\n",child->next->linenumber);
                        break;
                    }
                    depth+=1;
                    //printf("%d\n",depth);
                    tmp=tmp->children_queue;
                }
                else
                {
                    tmp=tmp->next->children_queue;
                }
            }
            
            //printf("%s\n",tmp->tag);
            if (strcmp(tmp->tag,"ID")||tmp->next)
            {
                semantic_error=1;
                printf("Error type 10 at Line %d:  not an array.\n",child->next->linenumber);
                fprintf(fw,"Error type 10 at Line %d:  not an array.\n",child->next->linenumber);
            }
            else
            {
                r=search_id(tmp->text);
                //printf("%d\n",r->isArray);
                if (!r||!r->isArray)
                {
                    semantic_error=1;
                    printf("Error type 10 at Line %d:  not an array.\n",child->next->linenumber);
                    fprintf(fw,"Error type 10 at Line %d:  not an array..\n",child->next->linenumber);
                }
                else
                {
                    struct array* list=r->array_shape;
                    //printf("%p\n",r->list);
                    while(list)
                    {
                        depth-=1;
                        list=list->next;
                    }
                    //printf("%d\n",depth);
                    if (depth>0)
                    {
                        //printf("%s\n",tmp->text);
                        semantic_error=1;
                        printf("Error type 10 at Line %d:  not an array.\n",child->next->linenumber);
                        fprintf(fw,"Error type 10 at Line %d:  not an array..\n",child->next->linenumber);
                    }
                    
                }
            }
            //printf("%s\n",child->next->next->children_queue->tag);
            if (strcmp(Exp(child->next->next),"int"))
            {
                semantic_error=1;
                printf("Error type 12 at Line %d:  index must be integer.\n",child->next->linenumber);
                fprintf(fw,"Error type 12 at Line %d:  index must be integer.\n",child->next->linenumber);
            }
            if (depth==0)
            {
                return r->type;
            }
            else
            {
                char* arr=(char*)malloc(10);
                strcpy(arr,"array:");
                sprintf(arr+6, "%d", depth);
                return arr;
            }
            return "none";
        }
        if (!strcmp(child->next->tag,"DOT"))
        {
            int struct_flag=0;
            TreeNode* tmp;
            if (!strcmp(child->children_queue->tag,"ID")||(!strcmp(child->children_queue,"LP")&&!strcmp(child->children_queue->next->children_queue->tag,"ID")))
            {
                struct_flag=1;
                tmp=(!strcmp(child->children_queue->tag,"ID"))?(child->children_queue):(child->children_queue->next->children_queue);
            }
            //printf("%d\n",struct_flag);
            if (!struct_flag)
            {
                semantic_error=1;
                printf("Error type 13 at Line %d:  Illegal use of \".\".\n",child->next->linenumber);
                fprintf(fw,"Error type 13 at Line %d:  Illegal use of \".\".\n",child->next->linenumber);
            }
            else
            {
                struct tableItem* sr=search_id(tmp->text);
                //printf("%p\n",sr);
                if (!sr)
                {
                    semantic_error=1;
                    printf("Error type 1 at Line %d: undefined variable: %s.\n",root->linenumber,tmp->text);
                    fprintf(fw,"Error type 1 at Line %d: undefined varibale: %s.\n",root->linenumber,tmp->text);
                }
                else
                {
                    //printf("%d\n",sr->isStruct);
                    if (!sr->isStruct)
                    {
                        semantic_error=1;
                        printf("Error type 13 at Line %d:  Illegal use of \".\".\n",child->next->linenumber);
                        fprintf(fw,"Error type 13 at Line %d:  Illegal use of \".\".\n",child->next->linenumber);
                    }
                    else
                    {
                        int find_flag=0;
                        //printf("%p\n",sr->struct_list->next->next);
                        //printf("%s\n",child->next->next->text);
                        //here
                        struct tableItem* slist=sr->struct_list;
                        
                        while(slist)
                        {
                            //printf("%s\n",slist->type);
                            //printf("%d\n",strcmp(slist->ID,child->next->next->text));
                            if (!strcmp(slist->ID,child->next->next->text))
                            {
                                find_flag=1;
                                return slist->type;
                            }
                            slist=slist->next;
                        }
                        if (!find_flag)
                        {
                            semantic_error=1;
                            printf("Error type 14 at Line %d:  Non-existent field:%s\n",child->next->linenumber,child->next->next->text);
                            fprintf(fw,"Error type 14 at Line %d:  Non-existent field:%s\n",child->next->linenumber,child->next->next->text);
                            return "none";
                        }
                        
                    }
                }
            }
        }
    }
    char* rs=(char*)malloc(strlen("none"));
    strcpy(rs,"none");
    return rs;
}
//real args
void Args(TreeNode* root,char** res,int i)
{
    res[i]=Exp(root->children_queue);
    if (root->children_queue->next)
    {
        Args(root->children_queue->next->next,res,i+1);
    }

}
//function sentences
void Stmt(TreeNode* root,char* type)
{
    
    TreeNode* child=root->children_queue;
    //printf("%s\n",child->tag);
    if (!strcmp(child->tag,"RETURN"))
    {
        //printf("%d\n",strcmp(Exp(child->next),type));
        if (strcmp(Exp(child->next),type))
        {
            semantic_error=1;
            printf("Error type 8 at Line %d: Type mismatched for return.\n",child->linenumber);
            fprintf(fw,"Error type 8 at Line %d: Type mismatched for return.\n",child->linenumber);
        }
        //printf("flag");
        return;
    }
    else if (!strcmp(child->tag,"Exp"))
    {
        Exp(child);
        return;
    }
    else if (!strcmp(child->tag,"IF"))
    {
        char* re=Exp(child->next->next);
        if (strcmp(re,"int"))
        {
            semantic_error=1;
            printf("Error type 7 at Line %d:  Type mismatched for operands.",child->linenumber);
            fprintf(fw,"Error type 7 at Line %d:  Type mismatched for operands.",child->linenumber);
        }
        Stmt(child->next->next->next->next,type);
        if (child->next->next->next->next->next)
        {
            Stmt(child->next->next->next->next->next->next,type);
        }
        return;
    }
    else if (!strcmp(child->tag,"WHILE"))
    {
        Exp(child->next->next);
        Stmt(child->next->next->next->next,type);
        return;
    }
    else
    {
        child=child->children_queue;
        DefList(child->next,NULL);
        return;
    }
}

void StmtList(TreeNode* root,char* type)
{
    //printf("%d\n",root->epi_flag);
    if (root->epi_flag)
    {
        return;
    }
    TreeNode* child=root->children_queue;
    
    Stmt(child,type);
    //printf("%s\n",type);
    StmtList(child->next,type);
}