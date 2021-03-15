#simple makefile
CFLAGS = -lpthread -Wall 
CC = gcc

main.out : src/main.c
	$(CC) $(CFLAGS) $< -o $@

clean :
	@echo 'Cleaning files...'
	@rm -f "/src/*.o" *.out
