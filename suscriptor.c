/* 
Autor: Mariela Curiel
Adaptación realizada por: Wilson Sanchez y Juan Manuel Durán
funcion: Suscriptor que envia uno de los dos topicos. Las llamadas al sistema no están validadas adecuadamente. 
Se invoca con el nombre del pipe que lo comunica al proceso central.c  y el topico (1 ó 2).  
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "nom.h"



int fd1;
char mensaje[TAMNEW];


int main (int argc, char **argv)
{
  int  fd, pid, creado = 0, cuantos=0;
  datap datos;
  char nombre[TAMNOMBRE];
  int cantidad;
  int topicos[5];
  char nombrepipesub[TAMNOMBRE];
 
  
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
 
   for(int i=0;i<TAMARGVSUB;i++){ 

     if((strcmp(argv[i], "-s") == 0)){
         printf("El nombre del archivo del pipe publicador es %s\n",argv[i+1]);
         strcpy(nombrepipesub,argv[i+1]);
      }
      if(i==3) break;
   }
         
    printf("Indique la cantidad de tematicas a las que se suscribira: ");
   scanf("%d",&cantidad); //sugerencia: cambiar los topicos por enteros
   printf("Se manejaran las siguientes nomenclaturas para los temas\n");
   printf("#1 para noticias de tipo arte\n");
   printf("#2 para noticias de tipo farandula y espectaculos\n");
   printf("#3 para noticias de tipo ciencia\n");
   printf("#4 para noticias de tipo politica\n");
   printf("#5 para noticias de tipo sucesos\n");
   for(int i=0;i<cantidad;i++){
      printf("Ingrese el numero de la tematica ");
      scanf("%d",&topicos[i]);
   }

   for(int i=0;i<NUMTOPICS;i++){
      if(topicos[i]!=TOPART||topicos[i]!=TOPCIENCIA||topicos[i]!=TOPFARANDULA||topicos[i]!=TOPPOLITICS||topicos[i]!=TOPSUCESO){
         topicos[i]=0; //0 será el caracter vacio
      }
   }

  // Se abre el pipe cuyo nombre se recibe como argumento del main. 
  while(creado!=1){ 
     fd = open(nombrepipesub, O_RDONLY);
     if (fd == -1) {
         perror("Suscriptor pipe");
         printf("Se volvera a intentar despues\n");
	 sleep(3);        
     } else creado = 1;
  } 

  datos.pid = getpid();
  for(int i=0;i<NUMTOPICS;i++){
     datos.topico[i]=topicos[i];
  }
  
  // Nombre de un segundo pipe
  sprintf(nombre, "receptor%d", (int) getpid());
  //printf("Nombre = %s\n", nombre);
  strcpy(datos.segundopipe, nombre);
  //strcpy(datos.segundopipe, "mypipe");
  // Se crea un segundo pipe para la comunicacion con el server.
 
  if (mkfifo (datos.segundopipe, fifo_mode) == -1) {
     perror("Client  mkfifo");
     exit(1);
  }
  // se envia el nombre del pipe al otro proceso. 
   if(write(fd, &datos , sizeof(datos))==-1){
      perror("error en la escritura");
      exit(-1);
   }

   // Se abre el segundo pipe
  creado = 0;
  do { 
     if ((fd1 = open(datos.segundopipe, O_RDONLY)) == -1) {
        perror(" Cliente  Abriendo el segundo pipe. Se volvera a intentar ");
        sleep(5);
     } else creado = 1; 
   } while (creado == 0);
 
  
   // Se leen varias noticias por el segundo pipe

	
  do {	
      cuantos = read(fd1, mensaje, TAMNEW);
      if (cuantos <= 0) break;
      printf("El proceso lee la noticia  %s \n", mensaje);

  } while (cuantos > 0);
 
  close(fd);
  close(fd1);
  unlink(nombre); 
  exit(0);
 
 //falta la terminacion de suscriptor
  
}
