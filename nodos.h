#ifndef NODOS_H
#define NODOS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>


typedef struct Comando
{
    char comando[256];
    struct Comando *siguiente;
} Comando_t;


void insertar_comando(Comando_t **cabeza, char texto[]);
void imprimir_lista(Comando_t *cabeza);
void liberar_lista(Comando_t **cabeza);
void parsear_comandos(char input[], Comando_t **lista);

#endif