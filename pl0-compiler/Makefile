CC = gcc
LEX = flex
LDLIBS = -ll
YACC = bison

EXT_TOKENTABLE = ../scanner/tokentable.h

all: pl0-ll-parser01
clean:
	rm -f pl0-ll-parser01 pl0-ll-parser01.o
	rm -f pl0-scanner-ll.c pl0-scanner-ll.o
	rm -f misc.o

pl0-ll-parser01: pl0-ll-parser01.o pl0-scanner-ll.o misc.o
pl0-ll-parser01.o: pl0-ll-parser01.c misc.h

misc.o: misc.c misc.h

pl0-scanner-ll.o: pl0-scanner-ll.c $(EXT_TOKENTABLE)
	$(CC) $(CFLAGS) -c -o $@ $<
pl0-scanner-ll.c: pl0-scanner.l
	$(LEX) $(LFLAGS) -t $< > $@
