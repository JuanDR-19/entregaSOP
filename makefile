all: publicador server

server: publicador.o nom.h
	gcc -o publicador publicador.o

server.o: publicador.c nom.h
	gcc -c publicador.c

client: suscriptor.o nom.h
	gcc -o suscriptor suscriptor.o

client.o: suscriptor.c nom.h
	gcc -c suscriptor.c

client: sistema.o nom.h
	gcc -o sistema sistema.o

client.o: sistema.c nom.h
	gcc -c sistema.c