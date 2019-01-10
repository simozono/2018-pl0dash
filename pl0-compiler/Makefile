CC = gcc
LEX = flex
LDLIBS = -ll
YACC = bison

EXT_TOKENTABLE = ../scanner/tokentable.h

all: pl0-ll-parser01 pl0-ll-parser02 pl0-ll-compiler01 pl0-ll-compiler02
clean:
	rm -f pl0-ll-parser01 pl0-ll-parser01.o
	rm -f pl0-ll-parser02 pl0-ll-parser02.o
	rm -f pl0-ll-compiler01 pl0-ll-compiler01.o
	rm -f pl0-ll-compiler02 pl0-ll-compiler02.o
	rm -f pl0-scanner-ll.c pl0-scanner-ll.o
	rm -f misc.o symbol_table01.o symbol_table02.o codegen.o

pl0-ll-parser01: pl0-ll-parser01.o pl0-scanner-ll.o misc.o
pl0-ll-parser01.o: pl0-ll-parser01.c misc.h
pl0-ll-parser02: pl0-ll-parser02.o pl0-scanner-ll.o misc.o symbol_table01.o
pl0-ll-parser02.o: pl0-ll-parser02.c misc.h symbol_table01.h
pl0-ll-compiler01: pl0-ll-compiler01.o pl0-scanner-ll.o misc.o symbol_table02.o codegen.o
pl0-ll-compiler01.o: pl0-ll-compiler01.c misc.h symbol_table02.h codegen.h
pl0-ll-compiler02: pl0-ll-compiler02.o pl0-scanner-ll.o misc.o symbol_table02.o codegen.o
pl0-ll-compiler02.o: pl0-ll-compiler02.c misc.h symbol_table02.h codegen.h

misc.o: misc.c misc.h
codegen.o: codegen.c codegen.h
symbol_table01.o: symbol_table01.c symbol_table01.h
symbol_table02.o: symbol_table02.c symbol_table02.h

pl0-scanner-ll.o: pl0-scanner-ll.c $(EXT_TOKENTABLE)
	$(CC) $(CFLAGS) -c -o $@ $<
pl0-scanner-ll.c: pl0-scanner.l
	$(LEX) $(LFLAGS) -t $< > $@
