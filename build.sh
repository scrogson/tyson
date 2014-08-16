#! /bin/bash
cc -std=c99 -Wall -ledit -lm src/parser.c src/mpc.c -o bin/tyson
