all: myshell

myshell: myshell.o LineParser.o
	gcc -g -Wall -o myshell myshell.o LineParser.o
myshell.o: myshell.c LineParser.h
	gcc -g -Wall -c -o myshell.o myshell.c

.PHONY: clean

clean:
	rm -f *.o myshell looper mypipe