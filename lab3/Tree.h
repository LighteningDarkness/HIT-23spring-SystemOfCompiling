extern int yylineno;
extern char* yytext;
typedef struct TreeNode
{
    int isTernimal;//0 stands for it is a ternimal symbol
    int linenumber;
    char* tag;
    int epi_flag;
    //use link table to save the children
    struct TreeNode* next;
    struct TreeNode* children_queue;
    //the content of the current node may be string, integer or float.
    union
    {
        char* text;
        int intvalue;
        float floatvalue;
    };
    //used for jump
    int t;
    int f;
    int n;
    int begin;  
}TreeNode;


TreeNode* create_node(int isTernimal,int linenumber,int epi,char* info,int num_args,...);

void preorder(TreeNode* root,int tab);