all: myshell looper mypipe

myshell: myshell.o LineParser.o
	gcc -g -Wall -o myshell myshell.o LineParser.o
myshell.o: myshell.c LineParser.h
	gcc -g -Wall -c -o myshell.o myshell.c

looper: Looper.o
	gcc -g -Wall -o looper Looper.o
Looper.o: Looper.c
	gcc -g -Wall -c -o Looper.o Looper.c

mypipe: mypipe.o
	gcc -g -Wall -o mypipe mypipe.o
mypipe.o: mypipe.c
	gcc -g -Wall -c -o mypipe.o mypipe.c

LineParser.o: LineParser.c LineParser.h
	gcc -g -Wall -c -o LineParser.o LineParser.c


.PHONY: clean

clean:
	rm -f *.o myshell looper mypipe