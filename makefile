all: SH
SH: list.o tree.o exec.o main.o
	gcc -o sh list.o tree.o exec.o main.o
main.o: main.c list.h tree.h exec.h
	gcc -c main.c
list.o: list.c list.h
	gcc -c list.c
tree.o: tree.c list.h tree.h
	gcc -c tree.c
exec.o: exec.c list.h tree.h exec.h
	gcc -c exec.c
clean:
	rm *.o