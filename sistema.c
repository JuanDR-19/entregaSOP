// Autor: Mariela Curiel
//Adaptación realizada por: Wilson Sanchez y Juan Manuel Durán
// Descripcion: ejemplo de un Publicador/Suscriptor. En este archivo se encuentran dos hilos aparte del hilo principal.
// Un hilo lee noticias del publicador y la pone en un buffer,  el principal está pendiente de la conexión de nuevos suscriptores y otro
// hilo se encarga de recoger las noticias del buffer y enviarlas a los suscriptores. 
// El programa se invoca de la siguiente forma:
// $central pipesus pipepub
// El primer argumento es el pipe de comunicación con los suscriptores y el segundo con el publicador.
//  Solo se manejan dos tópicos y se guarda únicamente la última noticia de cada tópico. Solo fue probado con un publicador que envía 10
//  noticias asociadas a dos topicos. 
//  El suscriptor no necesariamente reciben todas las noticias, dependerá de su tiempo de creación. 

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "nom.h" 

// El tamaño del BUFFER donde se ponen las noticias

#define TAMBUF 80

// Prototipo de las funciones

void *take(newp *);
void *put(newp *);
void Publicador(int *fd);
void CerrarPipesS();
void BuscarUltima(int, int);
void ColocarSuscriptor(int, char*);

//Variables globales 

newp BUFFER[TAMBUF]; // buffer donde se pondrán las noticias para consumo inmediato.
newp LASTNEW[NTOP]; // buffer donde se guarda la última noticia de cada tópico. 
int globalfd[NSUSCR]; // se guardan los file descriptor de los pipes asociados a los suscriptores.
topicos suscr[NSUSCR]; // topicos y pid de cada suscriptor.
int finProd = FALSE;
int finCons = FALSE; // Para el fin de la lectura de las noticias
int pcons=0, pprod=0;
sem_t s, espacios, elementos; // semaforos para la implementacion del buffer




// Función que cierra los pipes por donde el central envía datos a los suscriptores. 
void CerrarPipesS() { //esta funcion no se modifica
  int i;
  for(i = 0; i < NSUSCR; i++)
    if (globalfd[i] > 0) close(globalfd[i]);

}  


//Thread que lee del pipe del proceso publicador y escribe en el buffer de noticias. 
void Publicador(int *fd) {

  int cuantos; 
  newp new;
 
 do {
    cuantos = read (*fd, &new, sizeof(newp));
    if(cuantos==-1){
      perror("fallo al abrir el pipe del publicador");
      exit(-1);
    } 
    if (cuantos == 0) break;
    put(&new);
  } while (cuantos > 0);
 
 
  new.topico = 0; //zero es el numero de topicos vacios
  put(&new);
  printf("\n Se terminan de leer las noticias del Publicador");
  finProd = TRUE;
  pthread_exit(NULL); //verificar la cerrada de los hilos de manera correcta 
}


// Se busca la ultima noticia de un determinado tópico y se envía al fd cuya posición de recibe como primer argumento.
void BuscarUltima(int pos, int topico) {
//esta funcion ya esta terminada, falta implementarle un semaforo
for(int i=0;;i++){
    if (LASTNEW[i].topico== topico) { 
     printf("Buscando Ultima envia %s  \n", LASTNEW[i].noticia);
     write(globalfd[pos], LASTNEW[i].noticia, strlen(LASTNEW[i].noticia) + 1);
  } 
}
   

}  


// Esta función la invoca el hilo consumidor (el que toma las noticias del buffer, para enviarlas a los suscriptores
void ColocarSuscriptor(int topico, char *noticia) {

  int i=0, salir=FALSE;

  // Coloca la ultima noticia del topico en la estructura de datos correspondiente
   LASTNEW[topico-1].topico = topico; //hay que cambiar el llamado a llegar hasta la ultima noticia guardada
   strcpy(LASTNEW[topico-1].noticia, noticia);

   
    // Enviar la noticia por el pipe correspondiente.
    while ((globalfd[i] != 0) && (salir == FALSE)) {       
      if (suscr[i].topico == topico) { // enviar la noticia por el pipe correspondiente, cambiar a un strcmp()
         if(write(globalfd[i], noticia, strlen(noticia) + 1)==-1){
             perror("error al escribir dentro del archivo");
         } 
      }
      i++;
     }

}  


// Funcion para tomar datos del buffer de noticias
void *take(newp *e) {

  newp temp,*pe;

  pe = e; 
  int i=0;

  for (;;) {
    
    sem_wait(&elementos);
    sem_wait(&s);
    memcpy(&temp, &pe[pcons], sizeof(newp));
    pe[pcons].topico = 0; // para indicar que la posición está vacia
    pcons= (pcons + 1) % TAMBUF;
    if (temp.topico  == 0) { // el ultimo elemento 
      sem_post(&s);
      sem_post(&espacios);
      finCons == TRUE;
      break;
    } else {
      ColocarSuscriptor(temp.topico, temp.noticia);
      sem_post(&s);
      sem_post(&espacios);
    }  
    
  }
  

}  

// Funcion para colocar elementos del buffer. 
void *put(newp *e) {
      sem_wait(&espacios);
      sem_wait(&s);
      if (BUFFER[pprod].topico == 0) 
      memcpy(&BUFFER[pprod], e, sizeof(newp));
      pprod = (pprod + 1) % TAMBUF;
      sem_post(&s);
      sem_post(&elementos);
}  


int main (int argc, char **argv)
{
  char nombrepipepub[TAMNOMBRE];
  char nombrepipesub[TAMNOMBRE];
  int tiempo=0;
  int fd, fd1, pid, n, bytes, cuantos, creado,i, abiertos;
  datap datos,salida1,salida2;
  pthread_t threadpub;
  pthread_t threadsub;
 
   
  mode_t fifo_mode = S_IRUSR | S_IWUSR;

  // inicializacion de los semáforos siguiendo el algoritmo productor, consumidor
  sem_init(&s, 0, 1);
  sem_init(&espacios, 0, TAMBUF);
  sem_init(&elementos, 0, 0);
  
  // inicializacion de las estructuras de datos globales compartidas
  
  for (i=0; i < TAMBUF; i++) BUFFER[i].topico = 0; //caracter de topico vacio
  for (i=0; i < NSUSCR; i++) globalfd[i] = -1;
  for (i=0; i < NSUSCR; i++) suscr[i].topico = -1;
  for (i=0; i < NTOP; i++) LASTNEW[i].topico = -1;
  
 
  // Creacion del pipe del lado del publicador
  //Se debe de hacer algo similar a o realizado en el publicador con los flags

  for(int i=0;i<TAMARGVPUB;i++){ 

     if((strcmp(argv[i], "-p") == 0)){
         printf("El nombre del archivo del pipe publicador es %s\n",argv[i+1]);
         strcpy(nombrepipepub,argv[i+1]);
      }else{
        if((strcmp(argv[i], "-s") == 0)){
           strcpy(nombrepipesub,argv[i+1]);
           printf("El nombre del archivo del pipe suscriptor es es %s\n", argv[i+1]);
         }else{
              if((strcmp(argv[i], "-t") == 0)){
                 printf("time\n");
                 tiempo=atoi(argv[i+1]);
                 printf("El tiempo registrado es %i, despues de estos segundos se deja de esperar por mas publicadores \n", tiempo);
               }
            }
         }
         if(i==7) break;
   }

    if (mkfifo (nombrepipepub, fifo_mode) == -1) {
        perror("mkfifo");
        exit(1);
    } 

    fd1 = open (nombrepipepub, O_RDONLY);
      
      if (fd1 == -1) {
         perror("pipe publicador");
         exit (0);
      }

  // Creacion del pipe del lado del suscriptor

     if (mkfifo (nombrepipesub, fifo_mode) == -1) {
        perror("mkfifo");
        exit(1);
     }

     fd = open (nombrepipesub,O_RDONLY);
     
     if (fd == -1) {
       perror("pipe suscriptor");
       exit (0);
     }

  //fd es el pipe sub y fd1 es el pipe pub
     // El primer hilo, lee las noticias del pipe del publicador y las coloca en el BUFFER de noticias, el segundo hilo toma las noticias
     // del BUFFER y las envía a los suscriptores. 
     
     pthread_create(&threadpub, NULL, (void*) Publicador, (void*)&fd1);        
     pthread_create(&threadsub, NULL, (void*) take, (void*)BUFFER);
      
     pthread_join(threadpub, (void**)&fd1);
     pthread_join(threadsub, (void**)BUFFER);
  
     pthread_exit(NULL);
     pthread_exit(NULL);
  
     // El hilo principal se queda leyendo del pipe de los suscriptores.     
  
      for(i=0;;i++) {

       cuantos = read (fd, &datos, sizeof(datos));
   
       if (cuantos <= 0) break;
     
       do { 
         //lo que hace el globalfd en este if es guardar el file descriptor del nombre del publicador mandado
          if ((globalfd[i] = open(datos.segundopipe, O_WRONLY)) == -1) {
             perror("Server Abriendo el segundo pipe ");
             printf("Se volvera a intentar despues\n");
             sleep(2);         
          } else creado = 1; 
       }  while (creado == 0);
       suscr[i].pid = datos.pid;
       suscr[i].topico = datos.topico[i];    
       BuscarUltima(i,suscr[i].topico);
    }   // Fin del ciclo infinito
    

  // Cerrar y eliminar el pipe
      close(fd);
      close(fd1);
      unlink(nombrepipepub);
      unlink(nombrepipesub);
}
