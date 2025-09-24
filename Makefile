.PHONY: all clean

all: compile run

compile:
	gcc -Wall -Wextra -std=c99 -o main main.c

run:
	./main
