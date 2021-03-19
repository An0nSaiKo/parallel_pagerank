#simple makefile
CFLAGS = -pthread -Wall 
CC = gcc

all : main.out test

main.out : src/main.c
	$(CC) $(CFLAGS) $< -o $@

test : main.out
	sh test.sh main.out tests

clean :
	@echo 'Cleaning files...'
	@rm -f "/src/*.o" *.out output.txt
