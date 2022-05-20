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

//comprobar cambios

int mgetline (char *line, int max, FILE *f)
{

    if (fgets(line, max, f)== NULL)
       return(0);
    else return(strlen(line));

}

int main (int argc, char **argv)
{
  int  fd, fd1, n,res,creado=0;
  int tiempo=0;
  char nombrearch[TAMNOMBRE];
  datap datos;
  newp  noti;
  char *file_contents[MAXNEWS][TAMNEW]; //esto sigue sacandome error y no se que es
  char noticia[TAMNEW];
  int topico;
  char not[TAMNEW];
     
  for(int i=0;i<TAMARGV;i++){ 

     if((strcmp(argv[i], "-p") == 0)){
         printf("El nombre del archivo a abrir es %s",argv[i+1]);
         strcpy(nombrearch,argv[i+1]);
      }else{
        if((strcmp(argv[i], "-f") == 0)){
           strcpy(nombrearch,argv[i+1]);
           printf("El nombre del archivo a trabajar es %s\n", nombrearch);
         }else{
              if((strcmp(argv[i], "-t") == 0)){
                 printf("time\n");
                 tiempo=atoi(argv[i+1]);
                 printf("El tiempo registrado es %i, despues de estos segundos se enviaran noticia por noticia \n", tiempo);
               }
            }
         }
   }

   do {
      fd = open(nombrearch, O_WRONLY);
      if (fd == -1) {
         perror("Publicador pipe");
         printf(" Se volvera a intentar despues\n");
         sleep(5);        
      } else creado = 1;
   } while (creado == 0); 

    FILE* inputfile= fopen(nombrearch,"r"); 
    if (inputfile==NULL){
       perror("Fallo al abrir el archivo");
       exit(-1);
      }else{
         while (mgetline(noticia, sizeof(noticia), inputfile) > 0)  {
            sscanf(noticia, "%d: %s", &topico, not);
            printf("%s",noticia);
            if(write(fd, noticia , sizeof(noticia))==-1){
               perror("Error al escribir dentro del pipe");
               exit(-1);
            }
            sleep(tiempo);
         }
      }
   printf("El publicador terminó su trabajo"); //publicador termina
   exit(0);

}