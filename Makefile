#simple makefile
CFLAGS = -pthread -Wall 
CC = gcc

main.out : src/main.c
	$(CC) $(CFLAGS) $< -o $@

clean :
	@echo 'Cleaning files...'
	@rm -f "/src/*.o" *.out output.txt
