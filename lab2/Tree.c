#include "Tree.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern FILE *fw;
TreeNode* create_node(int isTernimal,int linenumber,int epi,char* info,int num_args,...)
{
    TreeNode* newnode=(TreeNode*)malloc(sizeof(TreeNode));
    newnode->isTernimal=isTernimal;
    newnode->linenumber=linenumber;
    newnode->next=NULL;
    newnode->epi_flag=epi;
    va_list list;
    TreeNode* tmp;
    TreeNode* pre;
    newnode->tag=(char*)malloc(strlen(info));
    strcpy(newnode->tag,info);
    int i;
    if (!isTernimal)
    {
        newnode->children_queue=NULL;
        if (!strcmp(info,"INT"))
        {
            newnode->intvalue=atoi(yytext);
        }
        else if (!strcmp(info,"FLOAT"))
        {
            newnode->floatvalue=(float)atof(yytext);
        }
        else
        {
            newnode->text=(char*)malloc(strlen(yytext));
            strcpy(newnode->text,yytext);
        }
    }
    else
    {
        if (!epi)
        {
            va_start(list,num_args);
            tmp=va_arg(list,TreeNode*);
            newnode->children_queue=tmp;
            pre=newnode->children_queue;
            for (i=1;i<num_args;i++)
            {
                tmp=(TreeNode*)va_arg(list,TreeNode*);
                //here
                pre->next=tmp;
                pre=tmp;
            }
            pre->next=NULL;
            va_end(list);
        }
        else
        {
            newnode->children_queue=NULL;
        }
    }
    return newnode;
}
void preorder(TreeNode* root,int tab)
{
    int i;
    TreeNode* tmp=root->children_queue;
    

    if (!root->epi_flag)
    {
        for (i=0;i<tab;i++)
        {
            fprintf(fw," ");
            printf(" ");
        }
    }
    if (root->isTernimal&&!root->epi_flag)
    {
        fprintf(fw,"%s (%d)\n",root->tag,root->linenumber);
        printf("%s (%d)\n",root->tag,root->linenumber);
    }
    if (!root->isTernimal)
    {
        if (!strcmp(root->tag,"TYPE")||!strcmp(root->tag,"ID"))
        {
            fprintf(fw,"%s: %s\n",root->tag,root->text);
            printf("%s: %s\n",root->tag,root->text);
        }
        else if (!strcmp(root->tag,"INT"))
        {
            fprintf(fw,"%s: %d\n",root->tag,root->intvalue);
            printf("%s: %d\n",root->tag,root->intvalue);
        }
        else if (!strcmp(root->tag,"FLOAT"))
        {
            fprintf(fw,"%s: %f\n",root->tag,root->floatvalue);
            printf("%s: %f\n",root->tag,root->floatvalue);
        }
        else
        {
            fprintf(fw,"%s\n",root->tag);
            printf("%s\n",root->tag);
        }
    }
    while (tmp)
    {
        preorder(tmp,tab+2);
        tmp=tmp->next;
    }
}