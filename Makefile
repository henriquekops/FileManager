
all: run

run: filesystem.o shell.o auxiliary.o manipulators.o
	gcc -o run filesystem.o shell.o auxiliary.o manipulators.o

filesystem.o: src/filesystem.c
	gcc -o filesystem.o -c src/filesystem.c -Iinc

shell.o: src/shell.c inc/shell.h
	gcc -o shell.o -c src/shell.c -Iinc

auxiliary.o: src/auxiliary.c inc/auxiliary.h
	gcc -o auxiliary.o -c src/auxiliary.c -Iinc

manipulators.o: src/manipulators.c inc/manipulators.h
	gcc -o manipulators.o -c src/manipulators.c -Iinc