CC = gcc
LEX = flex
YACC = bison

all: pl0-scanner
clean:
	rm -f pl0-scanner pl0-scanner.o pl0-scanner.c
pl0-scanner.o: pl0-scanner.c
	$(CC) $(CFLAGS) -c -o $@ $<
pl0-scanner.c: pl0-scanner.l
	$(LEX) $(LFLAGS) -t $< > $@
