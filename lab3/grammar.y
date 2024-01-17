%{
    #include <stdlib.h>
    #include <stdio.h>
    #include "Tree.h"
    TreeNode* root;
    #define YYSTYPE TreeNode*
    void yyerror(char* msg);
    extern int analysis_error;
    extern FILE* fw;
    extern int yylex();
%}

%token  INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%right ASSIGNOP
%type Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Dec Def DecList  Exp Args

%right NOT
%left RELOP PLUS MINUS STAR DIV AND OR DOT LP RP LB RB
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
/*High-level Definitions*/
/*the whole program*/
Program : ExtDefList{$$=create_node(1,@$.first_line,0,"Program",1,$1);root=$$;};
/*definition list*/
ExtDefList : ExtDef ExtDefList{$$=create_node(1,@$.first_line,0,"ExtDefList",2,$1,$2);}
             | {$$=create_node(1,@$.first_line,1,"ExtDefList",0);};
/*variable difinition sentences*/
ExtDef : Specifier ExtDecList SEMI{$$=create_node(1,@$.first_line,0,"ExtDef",3,$1,$2,$3);}
        | Specifier SEMI{$$=create_node(1,@$.first_line,0,"ExtDef",2,$1,$2);}
        | Specifier FunDec CompSt{$$=create_node(1,@$.first_line,0,"ExtDef",3,$1,$2,$3);}
        | Specifier error SEMI{analysis_error=1;yyerrok;};

/*one or more names of variables*/
ExtDecList : VarDec{$$=create_node(1,@$.first_line,0,"ExtDecList",1,$1);}
            | VarDec COMMA ExtDecList{$$=create_node(1,@$.first_line,0,"ExtDecList",3,$1,$2,$3);};

/*Specifiers*/
/*variavles type difinition, including struct and commom data type*/
Specifier : TYPE{$$=create_node(1,@$.first_line,0,"Specifier",1,$1);}
            | StructSpecifier{$$=create_node(1,@$.first_line,0,"Specifier",1,$1);};

StructSpecifier : STRUCT OptTag LC DefList RC{$$=create_node(1,@$.first_line,0,"StructSpecifier",5,$1,$2,$3,$4,$5);}
                | STRUCT Tag{$$=create_node(1,@$.first_line,0,"StructSpecifier",2,$1,$2);};
/*option, such as struct (name)*/
OptTag : ID{$$=create_node(1,@$.first_line,0,"OptTag",1,$1);}
        | {$$=create_node(1,@$.first_line,1,"OptTag",0);};
/*as its name*/
Tag : ID{$$=create_node(1,@$.first_line,0,"Tag",1,$1);};

/*Declarators*/
/*array and varibales, without assigning values*/
VarDec : ID{$$=create_node(1,@$.first_line,0,"VarDec",1,$1);}
        | VarDec LB INT RB{$$=create_node(1,@$.first_line,0,"VarDec",4,$1,$2,$3,$4);}
        | error RB{analysis_error=1;yyerrok;};
/*function*/
FunDec : ID LP VarList RP{$$=create_node(1,@$.first_line,0,"FunDec",4,$1,$2,$3,$4);}
        | ID LP RP{$$=create_node(1,@$.first_line,0,"FunDec",3,$1,$2,$3);}
        | ID error SEMI{analysis_error=1;yyerrok;}
        | error LP RP{analysis_error=1;yyerrok;}
        | ID LP error RP{analysis_error=1;yyerrok;};
/*variables list*/
VarList : ParamDec COMMA VarList{$$=create_node(1,@$.first_line,0,"VarList",3,$1,$2,$3);}
        | ParamDec{$$=create_node(1,@$.first_line,0,"VarList",1,$1);};

ParamDec : Specifier VarDec{$$=create_node(1,@$.first_line,0,"ParamDec",2,$1,$2);};

/*Statements*/
/*block*/
CompSt : LC DefList StmtList RC{$$=create_node(1,@$.first_line,0,"CompSt",4,$1,$2,$3,$4);}
        | error SEMI{analysis_error=1;yyerrok;}

StmtList : Stmt StmtList{$$=create_node(1,@$.first_line,0,"StmtList",2,$1,$2);}
        | {$$=create_node(1,@$.first_line,1,"StmtList",0);};

Stmt : Exp SEMI{$$=create_node(1,@$.first_line,0,"Stmt",2,$1,$2);}
    | CompSt{$$=create_node(1,@$.first_line,0,"Stmt",1,$1);}
    | RETURN Exp SEMI{$$=create_node(1,@$.first_line,0,"Stmt",3,$1,$2,$3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=create_node(1,@$.first_line,0,"Stmt",5,$1,$2,$3,$4,$5);}
    | IF LP Exp RP Stmt ELSE Stmt{$$=create_node(1,@$.first_line,0,"Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
    | WHILE LP Exp RP Stmt{$$=create_node(1,@$.first_line,0,"Stmt",5,$1,$2,$3,$4,$5);}
    | Exp error SEMI{analysis_error=1;yyerrok;};

/*Local Definitions*/
DefList : Def DefList{$$=create_node(1,@$.first_line,0,"DefList",2,$1,$2);}
        | {$$=create_node(1,@$.first_line,1,"DefList",0);};

Def : Specifier DecList SEMI{$$=create_node(1,@$.first_line,0,"Def",3,$1,$2,$3);}
    | Specifier error SEMI{analysis_error=1;yyerrok;};

DecList : Dec{$$=create_node(1,@$.first_line,0,"DecList",1,$1);}
        | Dec COMMA DecList{$$=create_node(1,@$.first_line,0,"DecList",3,$1,$2,$3);};

Dec : VarDec{$$=create_node(1,@$.first_line,0,"Dec",1,$1);}
    | VarDec ASSIGNOP Exp{$$=create_node(1,@$.first_line,0,"Dec",3,$1,$2,$3);};

/*Expressions*/
Exp : Exp ASSIGNOP Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp AND Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp OR Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp RELOP Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp PLUS Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp MINUS Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp STAR Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp DIV Exp{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | LP Exp RP{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | MINUS Exp{$$=create_node(1,@$.first_line,0,"Exp",2,$1,$2);}
    | NOT Exp{$$=create_node(1,@$.first_line,0,"Exp",2,$1,$2);}
    | ID LP Args RP{$$=create_node(1,@$.first_line,0,"Exp",4,$1,$2,$3,$4);}
    | ID LP RP{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | Exp LB Exp RB{$$=create_node(1,@$.first_line,0,"Exp",4,$1,$2,$3,$4);}
    | Exp DOT ID{$$=create_node(1,@$.first_line,0,"Exp",3,$1,$2,$3);}
    | ID{$$=create_node(1,@$.first_line,0,"Exp",1,$1);}
    | INT{$$=create_node(1,@$.first_line,0,"Exp",1,$1);}
    | FLOAT{$$=create_node(1,@$.first_line,0,"Exp",1,$1);}
    | ID LP error RP {analysis_error=1;yyerrok;};

Args : Exp COMMA Args{$$=create_node(1,@$.first_line,0,"Args",3,$1,$2,$3);}
    | Exp{$$=create_node(1,@$.first_line,0,"Args",1,$1);};

%%
void yyerror(char* msg)
{
    fprintf(fw,"Error type B at Line %d: %s.\n",yylineno,msg);
    printf("Error type B at Line %d: %s.\n",yylineno,msg);
}
