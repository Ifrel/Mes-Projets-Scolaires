CC=gcc
CFLAGS=-I.
DEPS = table_symboles.h
OBJS = table_symboles.o
LIBS = -lm

all: expr assign1 assign2

# Les programmes principaux
expr: expr.o expr_lexer.o $(OBJS)
	gcc -o expr expr.o expr_lexer.o $(OBJS) $(LIBS)

assign1: assign1.o assign1_lexer.o $(OBJS)
	gcc -o assign1 assign1.o assign1_lexer.o $(OBJS) $(LIBS)

assign2: assign2.o assign2_lexer.o $(OBJS)
	gcc -o assign2 assign2.o assign2_lexer.o $(OBJS) $(LIBS)

# Génération des analyseurs syntaxiques par bison
expr.h expr.c: expr.y
	bison expr.y --defines=expr.h -o expr.c

assign1.h assign1.c: assign1.y
	bison assign1.y --defines=assign1.h -o assign1.c -r all

assign2.h assign2.c: assign2.y
	bison assign2.y --defines=assign2.h -o assign2.c -r all

# Génération des analyseurs lexicaux par flex
expr_lexer.c expr_lexer.h: expr.lex
	flex --header-file=expr_lexer.h -oexpr_lexer.c expr.lex

assign1_lexer.c assign1_lexer.h: assign1.lex
	flex --header-file=assign1_lexer.h -oassign1_lexer.c assign1.lex

assign2_lexer.c assign2_lexer.h: assign2.lex
	flex --header-file=assign2_lexer.h -oassign2_lexer.c assign2.lex

# cibles standard
clean:
	rm -f $(OBJS) expr_lexer.c expr_lexer.h expr_lexer.o expr.c expr.h expr.o
	rm -f assign1_lexer.c assign1_lexer.h assign1_lexer.o assign1.c assign1.h assign1.o
	rm -f assign2_lexer.c assign2_lexer.h assign2_lexer.o assign2.c assign2.h assign2.o

distclean: clean
	rm -f expr assign1 assign2

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)