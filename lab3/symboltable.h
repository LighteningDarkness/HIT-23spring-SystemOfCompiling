struct tableItem
{
    char *type; 
    char* ID;
    int isFunc;
    int isStruct;
    int isArray;
    int lineno;
    char* struct_name;
    char* other_name;
    union 
    {
        int intvalue;
        float floatvalue;
    };
    struct tableItem* list;
    struct array* array_shape;
    struct tableItem* struct_list;
    
    struct tableItem* next;
};

// struct varList
// {
//     char *type;
//     char* ID;
//     struct varList* next;
// };
struct array
{
    int dim;
    union 
    {
        int intvalue;
        float floatvalue;
    };
    
    struct array* next;
};
struct tableItem* table[113];
int hashFunc(char* id);
void addItem(struct tableItem* item);
struct tableItem* search_struct_type(char* type,int *flag);
struct tableItem* search_id(char* id);
char* struct_type[100];
int len_struct_type;