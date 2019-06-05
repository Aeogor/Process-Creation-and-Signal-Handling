all: sig shell

sig: sig.c
	gcc -g -o sig sig.c

shell: shell.c
	gcc -g -o shell shell.c
