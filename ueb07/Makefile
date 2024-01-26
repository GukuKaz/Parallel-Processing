#!/usr/bin/make
TAR1 = ./bin/aufgabe1
TAR2 = ./bin/aufgabe2
SRC1 = aufgabe1.c
SRC2 = aufgabe2.c
OBJ1 = $(SRC1:%.c=%.o)
OBJ2 = $(SRC2:%.c=%.o)
CFLAGS	= -Wall -Wextra -pedantic-errors -g -fopenmp
LFLAGS  = -lm -fopenmp

%.o: %.c %.h
	mkdir -p ./bin
	$(CC) $(CFLAGS) -c $< -o ./bin/$@

%.o: %.c
	mkdir -p ./bin
	$(CC) $(CFLAGS) -c $< -o ./bin/$@

all: $(TAR1) $(TAR2)

$(TAR1): $(OBJ1) masspoint1.o
	mkdir -p ./bin
	$(CC) -o $@ ./bin/$(OBJ1) ./bin/masspoint1.o $(LFLAGS)
	rm -f *.gch

$(TAR2): $(OBJ2) masspoint2.o
	mkdir -p ./bin
	$(CC) -o $@ ./bin/$(OBJ2) ./bin/masspoint2.o $(LFLAGS)
	rm -f *.gch

run: all
	$(TAR1)
	$(TAR2)

clean:
	$(RM) -rf bin
