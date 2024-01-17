#include "Tree.h"


void semantic_preorder(TreeNode* root,struct tableItem table[]);



void ExtDecList(TreeNode* root,char* type);



struct tableItem* ParamDec(TreeNode* root);

struct tableItem* Dec(TreeNode* root,char* type,char* name);


struct tableItem* DecList(TreeNode* root,char* type,char* name);

struct tableItem* DefList(TreeNode* root,char* name);

struct tableItem* Def(TreeNode* root,char* name);



struct tableItem* VarList(TreeNode* root);


void ExtDef(TreeNode* root);
void struct_field_check(struct tableItem* list);
char* Exp(TreeNode* root);
void Args(TreeNode* root,char** res,int i);
void Stmt(TreeNode* root,char* type);
void StmtList(TreeNode* root,char* type);