/* 
Autor: Mariela Curiel
Adaptación realizada por: Wilson Sanchez y Juan Manuel Durán
funcion: Publicador que envia noticias asociadas a dos topicos. El nombre del pipe que lo comunica con  proceso central.c se recibe como argumento del main. 
No está validadas todas las llamadas al sistema. Este publicador envía 10 noticias. 
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "nom.h"

//cambio

int main (int argc, char **argv)
{
  int  fd, fd1, n, cuantos=0,res,creado=0;
  int tiempo=0;
  int j=0;
  char nombrearch[TAMNOMBRE];
  datap datos;
  newp  noti;
  char *file_contents[TAMNEW] = malloc(sizeof(newp)); //esto sigue sacandome error y no se que es
     
  for(int i=0;i<TAMARGV;i++){ 
     if((strcmp(argv[i], "-p") == 0)){
         printf("El nombre del archivo a abrir es %s",argv[i+1]);
         do {
            fd = open(argv[i+1], O_WRONLY);
            if (fd == -1) {
               perror("Publicador pipe");
               printf(" Se volvera a intentar despues\n");
               sleep(5);        
            } else creado = 1;
         } while (creado == 0);

     }else{
        if((strcmp(argv[i], "-f") == 0)){
           strcpy(nombrearch,argv[i+1]);
           printf("El nombre del archivo a trabajar es %s\n", nombrearch);
           int inputfile= open(nombrearch,O_RDONLY); //se abre el archivo de nombre especificado
           if (inputfile==-1){
              perror("Fallo al abrir el archivo");
              exit(EXIT_FAILURE);
           }else{
              
              do{
                 if(read(inputfile,file_contents[j],TAMNEW)==-1){
                    perror("Fallo al leer el archivo");
                    exit(-1);
                 }
                 cuantos++;
                 j++;
              }while(inputfile!=EOF); 
           }
           //todo lo que tenga que ver con la lectura del archivo de noticias, agregando la categoria a donde corresponda
        }else{
           if((strcmp(argv[i], "-t") == 0)){
              printf("time\n");
              tiempo=atoi(argv[i+1]);
              printf("El tiempo registrado es %i, despues de estos segundos se enviaran noticia por noticia \n", tiempo);
           }
        }
     }
  }

   for(int i=0;i<cuantos;i++){
       if(write(fd, &file_contents[i] , sizeof(newp))==-1){
          perror("Error al escribir dentro del pipe");
          exit(-1);
       }
       sleep(tiempo);
   }
   
   printf("El publicador terminó su trabajo"); //publicador termina
   exit(0);

}
    