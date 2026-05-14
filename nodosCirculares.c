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
    int waiting_time;
    int turnaround_time;
    struct Proceso *siguiente;
} Proceso_t;

void mkprocess(Proceso_t **cola, char id[], int burst)
{
    Proceso_t *nuevo = (Proceso_t *)malloc(sizeof(Proceso_t));
    strncpy(nuevo->id, id, sizeof(nuevo->id) - 1);
    nuevo->burst_time = burst;
    nuevo->waiting_time = 0;
    nuevo->turnaround_time = 0;

    if (*cola == NULL)
    {
        nuevo->siguiente = nuevo;
        *cola = nuevo;
    }
    else
    {
        Proceso_t *temp = *cola;

        while (temp->siguiente != *cola)
        {
            temp = temp->siguiente;
        }

        temp->siguiente = nuevo;
        nuevo->siguiente = *cola;
    }
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
    if (cola == NULL)
    {
        printf("Cola vacia\n");
        return;
    }

    Proceso_t *temp = cola;

    do
    {
        printf("[ID:%s | BT:%d] -> ", temp->id, temp->burst_time);
        temp = temp->siguiente;
    } while (temp != cola);

    printf("(vuelve al inicio)\n");
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