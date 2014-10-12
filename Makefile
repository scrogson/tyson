PROG=tyson
OBJECTS=
CFLAGS=-g -Wall -O3
LDFLAGS=-ledit -lm
INCLUDES=-I./includes
CC=cc -std=c99

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o bin/$@ src/parser.c src/mpc.c
