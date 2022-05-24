#define TAMMENSAJE 20
#define TAMNOMBRE 20
#define TAMNEW 80
#define NNOTICIAS 5
#define NTOP 2
#define NSUSCR 20
#define TRUE 1
#define FALSE 0
#define NUMTOPICS 5
#define TOPART 1
#define TOPFARANDULA 2
#define TOPCIENCIA 3
#define TOPPOLITICS 4
#define TOPSUCESO 5
#define TAMARGVPUB 7
#define MAXNEWS 30



// Estructuras para que el suscriptor envie el nombre del topico. 

typedef struct data {
    char segundopipe[TAMNOMBRE];
    int pid;
    int topico[NUMTOPICS];
} datap;

typedef struct data1 {
    int pid;
    int topico;
} topicos;

// Estructura para que el o los publicadores envien noticias

typedef struct new {
  char noticia[TAMNEW];
  int topico;
} newp;