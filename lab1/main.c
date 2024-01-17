#include "Tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern TreeNode* root;
extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE*);
extern void preorder(TreeNode*,int);
int analysis_error=0;
FILE *fw;
int main(int argc,char** argv)
{
    FILE *f;
    char tmp[100];
    char tmp1[100];
    strcpy(tmp1,"./results/");
    strcpy(tmp,"result-");
    strcat(tmp,argv[1]+16);
    strcat(tmp,".txt");
    strcat(tmp1,tmp);
    if (argc<=1)
    {
        return 1;
    }
    if (!(f=fopen(argv[1],"r")))
    {
        perror(argv[1]);
        return 1;
    }
    if (!(fw=fopen(tmp1,"w+")))
    {
        printf("failed to open file");
        return 1;
    }
    
    //fprintf(fw,"\nFILE: %s\n",argv[1]);
    yyrestart(f);
    yyparse();
    fclose(f);
    if (!analysis_error)
    {
        preorder(root,0);
    }
    
    return 0;
}
