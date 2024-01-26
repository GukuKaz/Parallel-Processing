SHELL		= /bin/sh
CC			= gcc
CFLAGS	= -Wall -Wextra -pedantic-errors -g -pthread


test: test.c aufgabe1.o aufgabe3.o
	-mkdir bin/
	$(CC) $(CFLAGS) -o bin/test test.c bin/aufgabe1.o bin/aufgabe3.o
	-rm *.gch

aufgabe1.o: aufgabe1.c aufgabe1.h
	-mkdir bin/
	$(CC) $(CFLAGS) -c aufgabe1.c -o bin/aufgabe1.o

aufgabe3.o: aufgabe3.c aufgabe3.h
		$(CC) $(CFLAGS) -c aufgabe3.c -o bin/aufgabe3.o
