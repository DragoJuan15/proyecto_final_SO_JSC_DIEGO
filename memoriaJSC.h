#ifndef MEMORIA_JSC_H
#define MEMORIA_JSC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char id[10];
    int tamano;
} ProcesoMem;

typedef struct Bloque {
    int dir_base;
    ProcesoMem* proceso; 
    int tamano;
    bool ocupado;
    struct Bloque* siguiente;
} Bloque;

Bloque* crear_bloque(int dir_base, int tamano);
int allocJSC(Bloque* cabeza, char* id, int tamano, char* estrategia);
void freeJSC(Bloque* cabeza, char* id);
void compactJSC(Bloque* cabeza);
void mstatusJSC(Bloque* cabeza);

Bloque* first_fit(Bloque* cabeza, int tamano);
Bloque* best_fit(Bloque* cabeza, int tamano);
Bloque* worst_fit(Bloque* cabeza, int tamano);

#endif