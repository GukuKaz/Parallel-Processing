#!/usr/bin/make
TAR1 = dining_philo1
TAR3 = dining_philo3
SRC1 = aufgabe_1.c
SRC3 = aufgabe_3.c
OBJ1 = $(SRC1:%.c=%.o)
OBJ3 = $(SRC3:%.c=%.o)
CFLAGS	= -Wall -Wextra -pedantic-errors -g
LFLAGS  = -pthread

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TAR1) $(TAR3)

$(TAR1): $(OBJ1)
	mkdir -p /bin
	$(CC) $(LFLAGS) -o $@ $^
	rm -f *.gch

$(TAR3): $(OBJ3)
	mkdir -p /bin
	$(CC) $(LFLAGS) -o $@ $^
	rm -f *.gch

run: all
	./$(TAR1)
	./$(TAR3)

clean:
	$(RM) $(OBJ1) $(TAR1) $(OBJ3) $(TAR3)
