PROG=tyson
OBJECTS=
CFLAGS= -g -Wall -O3
LDFLAGS=-ledit -lm
CC=cc -std=c99

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/$@ src/parser.c src/mpc.c
