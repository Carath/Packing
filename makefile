all: exec

exec: main.o
	gcc -Wall -O2 -o exec main.o -lm

main.o: main.c
	gcc -Wall -O2 -c main.c

clean:
	rm -rf *.o *.a exec
