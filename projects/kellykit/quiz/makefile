CFLAGS= -Wall -fpic -coverage -lm

.PHONY: all run clean

all: test run

test: testme.c
	gcc -o test -g testme.c $(CFLAGS)
	
run: test
	./test
	
clean:
	rm -f *.o test test.exe *.gcov *.gcda *gcno *.so *.out