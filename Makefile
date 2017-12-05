CC = gcc -g
LEX = flex
YACC = bison -y
LLIBS = 
OBJS = main.o y.tab.o lex.yy.o \
	tree.o typeCheck.o 

turing : $(OBJS) 
	$(CC) -o turing $(OBJS) $(LLIBS) 

lex.yy.c : scanner.l
	$(LEX) scanner.l
lex.yy.o : lex.yy.c 

y.tab.o : y.tab.c y.tab.h tree.h
y.tab.c : parser.y
	$(YACC) parser.y
y.tab.h : parser.y
	$(YACC) -d parser.y 

tree.o : tree.c tree.h



typeCheck.o : typeCheck.c tree.h symbolTable.h

main.o : main.c tree.h

clean :
	rm lex.yy.c y.tab.c *.o
	rm turing
