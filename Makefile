
all: run

run: filesystem.o shell.o auxiliary.o default.o
	gcc -o run filesystem.o shell.o auxiliary.o default.o

filesystem.o: src/filesystem.c
	gcc -o filesystem.o -c src/filesystem.c -Iinc

shell.o: src/shell.c inc/shell.h
	gcc -o shell.o -c src/shell.c -Iinc

auxiliary.o: src/auxiliary.c inc/auxiliary.h
	gcc -o auxiliary.o -c src/auxiliary.c -Iinc

default.o: src/default.c inc/default.h
	gcc -o default.o -c src/default.c -Iinc