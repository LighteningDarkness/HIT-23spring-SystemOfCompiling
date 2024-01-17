%option yylineno
%{
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include "Tree.h"
    #define YYSTYPE TreeNode*
    #include "grammar.tab.h"
    extern int analysis_error;
    extern FILE* fw;
    //comes from experiments guidence
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
%}

/*regular expressions*/
BINARY 0[Bb][01]+
OCTONARY 0[0-7]+
DECIMAL [1-9][0-9]*|0
HEXADECIMAL 0[xX][a-fA-F0-9]+
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT \!
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
INT {BINARY}|{OCTONARY}|{DECIMAL}|{HEXADECIMAL}
FLOAT (([0-9]*\.[0-9]+)|([0-9]+\.[0-9]*))([eE][+-]?[0-9]+)?
ID [a-z_A-Z][a-z_A-Z0-9]*
delim [ \t\r]
ws {delim}+
ENTER \n

%%
{ws} {/*ignore the spaces and enter*/}
{INT} {yylval=create_node(0,yylineno,0,"INT",0);return INT;}
{FLOAT} {yylval=create_node(0,yylineno,0,"FLOAT",0);return FLOAT;}

{SEMI} {yylval=create_node(0,yylineno,0,"SEMI",0);return SEMI;}
{COMMA} {yylval=create_node(0,yylineno,0,"COMMA",0);return COMMA;}
{ASSIGNOP} {yylval=create_node(0,yylineno,0,"ASSIGNOP",0);return ASSIGNOP;}
{RELOP} {yylval=create_node(0,yylineno,0,"RELOP",0);return RELOP;}
{PLUS} {yylval=create_node(0,yylineno,0,"PLUS",0);return PLUS;}
{MINUS} {yylval=create_node(0,yylineno,0,"MINUS",0);return MINUS;}
{STAR} {yylval=create_node(0,yylineno,0,"STAR",0);return STAR;}
{DIV} {yylval=create_node(0,yylineno,0,"DIV",0);return DIV;}
{AND} {yylval=create_node(0,yylineno,0,"AND",0);return AND;}
{OR} {yylval=create_node(0,yylineno,0,"OR",0);return OR;}
{DOT} {yylval=create_node(0,yylineno,0,"DOT",0);return DOT;}
{NOT} {yylval=create_node(0,yylineno,0,"NOT",0);return NOT;}
{TYPE} {yylval=create_node(0,yylineno,0,"TYPE",0);return TYPE;}
{LP} {yylval=create_node(0,yylineno,0,"LP",0);return LP;}
{RP} {yylval=create_node(0,yylineno,0,"RP",0);return RP;}
{LB} {yylval=create_node(0,yylineno,0,"LB",0);return LB;}
{RB} {yylval=create_node(0,yylineno,0,"RB",0);return RB;}
{LC} {yylval=create_node(0,yylineno,0,"LC",0);return LC;}
{RC} {yylval=create_node(0,yylineno,0,"RC",0);return RC;}
{STRUCT} {yylval=create_node(0,yylineno,0,"STRUCT",0);return STRUCT;}
{RETURN} {yylval=create_node(0,yylineno,0,"RETURN",0);return RETURN;}
{IF} {yylval=create_node(0,yylineno,0,"IF",0);return IF;}
{ELSE} {yylval=create_node(0,yylineno,0,"ELSE",0);return ELSE;}
{WHILE} {yylval=create_node(0,yylineno,0,"WHILE",0);return WHILE;}
{ID} {yylval=create_node(0,yylineno,0,"ID",0);return ID;}
{ENTER} {yycolumn=1;}
 /*if lex is not equal to the above symbols then we can judge it is an error.*/
. {analysis_error=1;
   fprintf(fw,"Error type A at Line %d: Mysterious character \"%s\".\n",yylineno,yytext);
   printf("Error type A at Line %d: Mysterious character \"%s\".\n",yylineno,yytext);}
%%

