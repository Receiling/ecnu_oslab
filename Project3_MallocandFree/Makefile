libmem:mem
	gcc -shared -o libmem.so mem.o

mem:mem.h mem.c
	gcc -c -fpic mem.c -Wall -Werror
