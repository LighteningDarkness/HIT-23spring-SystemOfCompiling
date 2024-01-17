#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"
extern int semantic_error;
extern FILE *fw;

//hash function
int hashFunc(char* id)
{
    //printf("%s\n",id);
    if (!id)
    {
        return 0;
    }
    int l=(int)strlen(id);
    int sum=0;
    int i;
    for (i=0;i<l;i++)
    {
        sum+=(int)id[i];
    }
    return sum%91;
}
void initial()
{
    int i;
    for (i=0;i<113;i++)
    {
        table[i]=NULL;
    }
}
void initial_1()
{
    int i;
    for (i=0;i<100;i++)
    {
        struct_type[i]=NULL;
    }
}
void addItem(struct tableItem* item)
{
    // if (item->struct_list)
    // {
    //     printf("%s\n",item->struct_list->ID);
    // }
    int index=hashFunc(item->ID);
    struct tableItem* tmp;
    struct tableItem*  pre;
    item->next=NULL;
    //printf("%s\n",item->ID);
    //printf("%p\n",table[index]);
    if (!table[index])
    {
        //printf("%s\n",item->ID);
        table[index]=item;
    }
    else
    {
        //TODO
        tmp=table[index];
        pre=(struct tableItem*)malloc(sizeof(struct tableItem));
        while (tmp)
        {
            
            if (tmp->ID&&!strcmp(tmp->ID,item->ID))
            {
                if (item->isFunc)
                {
                    semantic_error=1;
                    printf("Error type 4 at Line %d: repeated function defination: \"%s\".\n",item->lineno,item->ID);
                    fprintf(fw,"Error type 4 at Line %d: repeated function defination: \"%s\".\n",item->lineno,item->ID);
                }
                else if (item->isStruct&&!item->isArray)
                {
                    semantic_error=1;
                    printf("Error type 16 at Line %d: repeated struct defination: \"%s\".\n",item->lineno,item->ID);
                    fprintf(fw,"Error type 16 at Line %d: repeated struct defination: \"%s\".\n",item->lineno,item->ID);
                }
                else
                {
                    //if they are both in the same struct type, then ignore.
                    if (!tmp->struct_name||!item->struct_name||strcmp(tmp->struct_name,item->struct_name))
                    {
                        semantic_error=1;
                        printf("Error type 3 at Line %d: repeated variable defination: \"%s\".\n",item->lineno,item->ID);
                        fprintf(fw,"Error type 3 at Line %d: repeated variable defination: \"%s\".\n",item->lineno,item->ID);
                    }
                    
                }
            }
            pre=tmp;
            tmp=tmp->next;
        }

        pre->next=item;
    }
}

struct tableItem* search_struct_type(char* type,int *flag)
{
    int i;
    for (i=0;i<91;i++)
    {
        if (table[i])
        {
            struct tableItem* tmp=table[i];
            while(tmp)
            {
                if (tmp->isStruct&&!strcmp(tmp->type,type))
                {
                    *flag=1;
                    //printf("%s\n",tmp->struct_list->next->ID);
                    return tmp->struct_list;
                }
                tmp=tmp->next;
            }
        }
    }
    return NULL;
}

struct tableItem* search_id(char* id)
{
    int index=hashFunc(id);
    struct tableItem* tmp=table[index];
    while (tmp)
    {
        if (tmp->ID&&!strcmp(tmp->ID,id))
        {
            //printf("%s\n",tmp->ID);
            //printf("%p\n",tmp->struct_list);
            return tmp;
        }
        tmp=tmp->next;
    }
    return NULL;
}