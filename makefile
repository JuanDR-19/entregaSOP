all: publicador suscriptor sistema

publicador: publicador.o nom.h
	gcc -o publicador publicador.o

publicador.o: publicador.c
	gcc -c -o publicador.o publicador.c

suscriptor: suscriptor.o nom.h
	gcc -o suscriptor suscriptor.o

suscriptor.o: suscriptor.c
	gcc -c -o suscriptor.o suscriptor.c

sistema: sistema.o nom.h
	gcc -o sistema sistema.o -pthread

sistema.o: sistema.c
	gcc -c -o sistema.o sistema.c