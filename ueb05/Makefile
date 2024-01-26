#!/usr/bin/make
TAR1 = ./bin/conpro1
TAR2 = ./bin/conpro2
SRC1 = aufgabe1.c
SRC2 = aufgabe2.c
OBJ1 = $(SRC1:%.c=%.o)
OBJ2 = $(SRC2:%.c=%.o)
CFLAGS	= -Wall -Wextra -pedantic-errors -g
LFLAGS  = -pthread

%.o: %.c %.h
	mkdir -p ./bin
	$(CC) $(CFLAGS) -c $< -o ./bin/$@

%.o: %.c
	mkdir -p ./bin
	$(CC) $(CFLAGS) -c $< -o ./bin/$@

all: $(TAR1) $(TAR2)

$(TAR1): $(OBJ1)
	mkdir -p ./bin
	$(CC) $(LFLAGS) -o $@ ./bin/$^
	rm -f *.gch

$(TAR2): $(OBJ2)
	mkdir -p ./bin
	$(CC) $(LFLAGS) -o $@ ./bin/$^
	rm -f *.gch

run: all
	$(TAR1)
	$(TAR2)

clean:
	$(RM) -rf bin
