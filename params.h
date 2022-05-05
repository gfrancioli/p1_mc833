#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX 2000
#define PORT 8080

static const char CADASTRO[MAX] = "1";
static const char LISTA_TODOS[MAX] = "2";
static const char LISTA_OPT[MAX] = "3";
static const char DELETE[MAX] = "4";
static const char GENERO[MAX] = "5";

typedef struct _movie {
    char id[MAX];
    char titulo[MAX];
    char genero[MAX];
    char diretor[MAX];
    char ano_lanc[MAX];
    char deleted[MAX];
} movie;