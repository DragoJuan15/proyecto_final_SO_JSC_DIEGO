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

typedef struct Proceso
{
    char id[50];
    int burst_time;
    int tamano_memoria; 
    char estado[20];     
    int waiting_time;
    int turnaround_time;
    struct Proceso *siguiente;
} Proceso_t;

void mkprocess(Proceso_t **cola, char id[], int burst, int tamano)
{
    Proceso_t *nuevo = (Proceso_t *)malloc(sizeof(Proceso_t));
    strncpy(nuevo->id, id, sizeof(nuevo->id) - 1);
    nuevo->burst_time = burst;
    nuevo->tamano_memoria = tamano; 
    strcpy(nuevo->estado, "NEW");  
    nuevo->waiting_time = 0;
    nuevo->turnaround_time = 0;

    if (*cola == NULL) {
        nuevo->siguiente = nuevo;
        *cola = nuevo;
    } else {
        Proceso_t *temp = *cola;
        while (temp->siguiente != *cola) temp = temp->siguiente;
        temp->siguiente = nuevo;
        nuevo->siguiente = *cola;
    }

    printf("Proceso creado exitosamente: ID=%s, Burst=%d, Memoria=%d unidades (Estado: NEW).\n", 
            nuevo->id, nuevo->burst_time, nuevo->tamano_memoria);
}


void eliminar_proceso(Proceso_t **cola, char id[])
{
    if (*cola == NULL)
    {
        return;
    }

    Proceso_t *actual = *cola;
    Proceso_t *prev = NULL;

    do
    {
        if (strcmp(actual->id, id) == 0)
        {
            if (actual == *cola && actual->siguiente == *cola)
            {
                free(actual);
                *cola = NULL;
                return;
            }

            if (actual == *cola)
            {
                Proceso_t *ultimo = *cola;
                while (ultimo->siguiente != *cola)
                {
                    ultimo = ultimo->siguiente;
                }
                *cola = actual->siguiente;
                ultimo->siguiente = *cola;
            }
            else{
                prev->siguiente = actual->siguiente;
            }

            free(actual);
            return;
        }

        prev = actual;
        actual = actual->siguiente;
    } while (actual != *cola);

    return;
}

void lstprocss(Proceso_t *cola)
{
    if (cola == NULL) {
        printf("Cola vacia\n");
        return;
    }
    printf("\n%-10s %-10s %-10s %-10s\n", "ID", "Burst", "Bloques", "Estado");
    printf("--------------------------------------------\n");
    Proceso_t *temp = cola;
    do {
        printf("%-10s %-10d %-10d %-10s\n", temp->id, temp->burst_time, temp->tamano_memoria, temp->estado);
        temp = temp->siguiente;
    } while (temp != cola);
}

void liberar_cola(Proceso_t **cola)
{
    if (*cola == NULL)
    {
        return;
    }

    Proceso_t *actual = *cola;
    Proceso_t *sig;

    do
    {
        sig = actual->siguiente;
        free(actual);
        actual = sig;
    } while (actual != *cola);

    *cola = NULL;
}

int buscar_id(Proceso_t *cola, char id[])
{
    if (cola == NULL)
    {
        return 0;
    }

    Proceso_t *temp = cola;

    do
    {
        if (strcmp(temp->id, id) == 0)
        {
            printf("ID repetido\n");
            return 1;
        }

        temp = temp->siguiente;

    } while (temp != cola);

    return 0;
}