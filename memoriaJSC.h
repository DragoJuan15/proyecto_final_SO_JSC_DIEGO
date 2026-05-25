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

// Prototipos
Bloque* crear_bloque(int dir_base, int tamano);
void allocJSC(Bloque* cabeza, char* id, int tamano, char* estrategia);
void freeJSC(Bloque* cabeza, char* id);
void compactJSC(Bloque* cabeza);
void mstatusJSC(Bloque* cabeza);

Bloque* crear_bloque(int dir_base, int tamano) {
    Bloque* nuevo = (Bloque*)malloc(sizeof(Bloque));
    nuevo->dir_base = dir_base;
    nuevo->tamano = tamano;
    nuevo->ocupado = false;
    nuevo->proceso = NULL;
    nuevo->siguiente = NULL;
    return nuevo;
}

//algoritmos de búsqueda
Bloque* first_fit(Bloque* cabeza, int tamano) {
    Bloque* actual = cabeza;
    while (actual != NULL) {
        if (!actual->ocupado && actual->tamano >= tamano) return actual;
        actual = actual->siguiente;
    }
    return NULL;
}

Bloque* best_fit(Bloque* cabeza, int tamano) {
    Bloque* mejor = NULL;
    Bloque* actual = cabeza;
    while (actual != NULL) {
        if (!actual->ocupado && actual->tamano >= tamano) {
            if (mejor == NULL || actual->tamano < mejor->tamano) mejor = actual;
        }
        actual = actual->siguiente;
    }
    return mejor;
}

Bloque* worst_fit(Bloque* cabeza, int tamano) {
    Bloque* peor = NULL;
    Bloque* actual = cabeza;
    while (actual != NULL) {
        if (!actual->ocupado && actual->tamano >= tamano) {
            if (peor == NULL || actual->tamano > peor->tamano) peor = actual;
        }
        actual = actual->siguiente;
    }
    return peor;
}

void allocJSC(Bloque* cabeza, char* id, int tamano, char* estrategia) {
    Bloque* elegido = NULL;
    if (strcmp(estrategia, "FF") == 0) elegido = first_fit(cabeza, tamano);
    else if (strcmp(estrategia, "BF") == 0) elegido = best_fit(cabeza, tamano);
    else if (strcmp(estrategia, "WF") == 0) elegido = worst_fit(cabeza, tamano);

    if (elegido) {
        if (elegido->tamano > tamano) {
            Bloque* sobrante = crear_bloque(elegido->dir_base + tamano, elegido->tamano - tamano);
            sobrante->siguiente = elegido->siguiente;
            elegido->siguiente = sobrante;
            elegido->tamano = tamano;
        }
        elegido->ocupado = true;
        elegido->proceso = (ProcesoMem*)malloc(sizeof(ProcesoMem));
        strcpy(elegido->proceso->id, id);
        elegido->proceso->tamano = tamano;
        printf("Proceso %s asignado en memoria.\n", id);
    } else {
        printf("Error: No hay memoria suficiente (Estrategia: %s).\n", estrategia);
    }
}

void freeJSC(Bloque* cabeza, char* id) {
    Bloque* actual = cabeza;
    while (actual != NULL) {
        if (actual->ocupado && strcmp(actual->proceso->id, id) == 0) {
            free(actual->proceso);
            actual->proceso = NULL;
            actual->ocupado = false;
            printf("Memoria del proceso %s liberada.\n", id);
            return;
        }
        actual = actual->siguiente;
    }
    printf("Proceso %s no estaba en memoria.\n", id);
}

void compactJSC(Bloque* cabeza) {
    Bloque* actual = cabeza;
    while (actual != NULL && actual->siguiente != NULL) {
        if (!actual->ocupado && !actual->siguiente->ocupado) {
            Bloque* temp = actual->siguiente;
            actual->tamano += temp->tamano;
            actual->siguiente = temp->siguiente;
            free(temp);
        } else {
            actual = actual->siguiente;
        }
    }
    printf("Compactacion finalizada.\n");
}

void mstatusJSC(Bloque* cabeza) {
    printf("\n--- MAPA DE MEMORIA ---\n");
    printf("%-10s %-10s %-10s %-10s\n", "Base", "Tamano", "Estado", "ID");
    Bloque* actual = cabeza;
    while (actual != NULL) {
        printf("%-10d %-10d %-10s %-10s\n", 
            actual->dir_base, actual->tamano,
            actual->ocupado ? "OCUPADO" : "LIBRE",
            actual->ocupado ? actual->proceso->id : "---");
        actual = actual->siguiente;
    }
}

#endif