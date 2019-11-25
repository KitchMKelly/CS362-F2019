CFLAGS= -Wall -fpic -coverage -lm -std=c99

all: unittestresults

unittestresults: unittest1 unittest2 unittest3 unittest4 unittest5
	./unittest1 >> unittestresults.out
	./unittest2 >> unittestresults.out
	./unittest3 >> unittestresults.out
	./unittest4 >> unittestresults.out
	./unittest5 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	cat dominion.c.gcov >> unittestresults.out

unittest1: unittest1.c dominion.o rngs.o
	gcc -o unittest1 -g unittest1.c dominion.o rngs.o $(CFLAGS)

unittest2: unittest2.c dominion.o rngs.o
	gcc -o unittest2 -g unittest2.c dominion.o rngs.o $(CFLAGS)
	
unittest3: unittest3.c dominion.o rngs.o
	gcc -o unittest3 -g unittest3.c dominion.o rngs.o $(CFLAGS)
	
unittest4: unittest4.c dominion.o rngs.o
	gcc -o unittest4 -g unittest4.c dominion.o rngs.o $(CFLAGS)
	
unittest5: unittest5.c dominion.o rngs.o 
	gcc -o unittest5 -g unittest5.c dominion.o rngs.o $(CFLAGS)

rngs.o: rngs.c rngs.h
	gcc -c rngs.c -g  $(CFLAGS)

dominion.o: dominion.c dominion.h rngs.o
	gcc -c dominion.c -g  $(CFLAGS)

clean:
	rm -f *.o unittest5 unittest5.ext unittest4 unittest4.exe unittest3 unittest3.exe unittest2 unittest2.exe unittest1 unittest1.exe *.gcov *.gcda *gcno *.so *.out