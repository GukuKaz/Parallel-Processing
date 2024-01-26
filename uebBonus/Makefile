#!/usr/bin/make
TAR = ./bin/aufgabe
SRC = aufgabe.c
OBJ = $(SRC:%.c=%.o)
CFLAGS	= -Wall -std=c11 -Wextra -pedantic-errors -g
LFLAGS  = -lm
CC = mpicc

%.o: %.c %.h
	mkdir -p ./bin
	$(CC) $(CFLAGS) -c $< -o ./bin/$@

%.o: %.c
	mkdir -p ./bin
	$(CC) $(CFLAGS) -c $< -o ./bin/$@

all: $(TAR)

$(TAR): $(OBJ) masspoint.o
	mkdir -p ./bin
	$(CC) -o $@ ./bin/$(OBJ) ./bin/masspoint.o $(LFLAGS)
	rm -f *.gch

run: all
	mpirun -machinefile Machinefile -np 10 -mca btl ^openib bin/aufgabe

clean:
	$(RM) -rf bin
