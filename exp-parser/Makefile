CC = gcc
LEX = flex
LDLIBS = -ll
YACC = bison

EXT_TOKENTABLE = ../scanner/tokentable.h

all: exp-ll-parser01 exp-ll-parser02 exp-lr-parser01 exp-lr-parser02
tests:
	./exp-ll-parser01 ../sample/exp01.txt
clean:
	rm -f *.o
	rm -f exp-ll-parser01 exp-ll-parser02
	rm -f exp-lr-parser01 exp-lr-parser02
	rm -f exp-scanner-ll.c exp-scanner-lr.c
	rm -f exp-lr-parser01.c exp-lr-parser02.c tokentable_bison.h

exp-ll-parser01: exp-ll-parser01.o exp-scanner-ll.o
exp-ll-parser02: exp-ll-parser02.o exp-scanner-ll.o
exp-ll-parser01.o: exp-ll-parser01.c $(EXT_TOKENTABLE)
exp-ll-parser02.o: exp-ll-parser02.c $(EXT_TOKENTABLE)

exp-lr-parser01: exp-lr-main.o exp-lr-parser01.o exp-scanner-lr.o
exp-lr-parser02: exp-lr-main.o exp-lr-parser02.o exp-scanner-lr.o
exp-lr-parser01.o: exp-lr-parser01.c tokentable_bison.h
exp-lr-parser02.o: exp-lr-parser02.c tokentable_bison.h


exp-scanner-ll.c: exp-scanner.l
	$(LEX) $(LFLAGS) -t $< > $@
exp-scanner-ll.o: exp-scanner-ll.c $(EXT_TOKENTABLE)
	$(CC) $(CFLAGS) -c -o $@ $<

exp-scanner-lr.c: exp-scanner.l
	$(LEX) -DYACC $(LFLAGS) -t $< > $@
exp-scanner-lr.o: exp-scanner-lr.c tokentable_bison.h
	$(CC) $(CFLAGS) -DYACC -c -o $@ $<
exp-lr-parser01.c + tokentable_bison.h: exp-lr-parser01.y
	$(YACC) --defines=tokentable_bison.h -o exp-lr-parser01.c $<
exp-lr-parser02.c + tokentable_bison.h: exp-lr-parser02.y
	$(YACC) --defines=tokentable_bison.h -o exp-lr-parser02.c $<
