all: publicador suscriptor sistema

publicador: publicador.o nom.h
	gcc -o publicador publicador.o -g

publicador.o: publicador.c
	gcc -c -o publicador.o publicador.c -g

suscriptor: suscriptor.o nom.h
	gcc -o suscriptor suscriptor.o -g

suscriptor.o: suscriptor.c
	gcc -c -o suscriptor.o suscriptor.c -g

sistema: sistema.o nom.h
	gcc -o sistema sistema.o -pthread -g

sistema.o: sistema.c
	gcc -c -o sistema.o sistema.c -g