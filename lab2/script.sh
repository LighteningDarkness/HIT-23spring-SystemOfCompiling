echo "generating lex.yy.c"
flex lex.l
echo "done"
echo "use bison to generate grammar.tab.c and grammar.tab.h"
bison -d grammar.y
echo "done"
echo "building..."
gcc -lfl main.c lex.yy.c grammar.tab.c Tree.c semanticChecking.c symboltable.c -o parser
echo "done"
echo "start to generate results..."
echo ""
for file in $(ls ./test_examples*)
do
    echo $file
    ./parser ./test_examples/$file
    echo ""
done