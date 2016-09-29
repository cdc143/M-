LEX = flex
CC = gcc

all: scanner

lex.yy.c: lexer.l
	$(LEX) lexer.l

scanner: lex.yy.c scanner.c util.c
	$(CC) -o scanner scanner.c util.c lex.yy.c cgen.c


clean:
	$(RM) lex.yy.c lexer.c scanner
