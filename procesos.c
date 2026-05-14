#include <stdio.h>
#include <stdlib.h>
#include "procesos.h"

void fcfsJSC(Proceso_t **cola)
{
    int wait = 0;
    int turn = 0;

    while (*cola != NULL)
    {
        wait = turn - (*cola)->waiting_time;
        turn = turn + (*cola)->burst_time;

        (*cola)->waiting_time = wait;
        (*cola)->turnaround_time = turn;

        printf("%s - wait %d - turn %d - burst %d\n", (*cola)->id, (*cola)->waiting_time, (*cola)->turnaround_time, (*cola)->burst_time);
        eliminar_proceso(cola, (*cola)->id);
    }
}

void sjfJSC(Proceso_t **cola, int preemptivo)
{
    int wait = 0;
    int turn = 0;

    if (preemptivo == 1)
    {
        if ((*cola)->burst_time == 1)
        {
            turn += 1;
            printf("%s - wait %d - turn %d - burst %d\n", (*cola)->id, wait, turn, (*cola)->burst_time);
            eliminar_proceso(cola, (*cola)->id);
        }
        else
        {
            turn += 1;
            printf("%s - wait %d - turn %d - burst %d\n", (*cola)->id, wait, turn, (*cola)->burst_time);
            (*cola)->burst_time -= 1;
            (*cola)->waiting_time += 1;
        }
        wait += 1;
    }

    while (*cola != NULL)
    {
        Proceso_t *temp = *cola;
        Proceso_t *menor = *cola;

        do
        {
            if (temp->burst_time < menor->burst_time)
            {
                menor = temp;
            }
            temp = temp->siguiente;
        } while (temp != *cola);

        wait = turn - menor->waiting_time;
        turn = turn + menor->burst_time;

        menor->waiting_time = wait;
        menor->turnaround_time = turn;

        printf("%s - wait %d - turn %d - burst %d\n", menor->id, menor->waiting_time, menor->turnaround_time, menor->burst_time);

        if (menor == *cola)
        {
            eliminar_proceso(cola, menor->id);
        }
        else
        {
            Proceso_t *prev = *cola;
            while (prev->siguiente != menor)
            {
                prev = prev->siguiente;
            }
            prev->siguiente = menor->siguiente;
            free(menor);
        }
    }
}

void rrJSC(Proceso_t **cola, int quantum)
{
    int tiempo_global = 0;

    while (*cola != NULL)
    {
        int tiempo = 0;

        if ((*cola)->burst_time > quantum)
        {
            tiempo = quantum;

            tiempo_global += tiempo;

            (*cola)->burst_time -= quantum;

            printf("%s - wait %d - turn %d - burst %d\n",(*cola)->id, (*cola)->waiting_time, tiempo_global, (*cola)->burst_time);

            aumentar_waiting(*cola, *cola, tiempo);

            *cola = (*cola)->siguiente;
        }
        else
        {
            tiempo = (*cola)->burst_time;

            tiempo_global += tiempo;

            printf("%s - wait %d - turn %d - burst %d\n", (*cola)->id, (*cola)->waiting_time, tiempo_global, 0);

            aumentar_waiting(*cola, *cola, tiempo);

            eliminar_proceso(cola, (*cola)->id);

            if (*cola == NULL)
                return;
        }
    }
}

void aumentar_waiting(Proceso_t *cola, Proceso_t *actual, int tiempo)
{
    if (cola == NULL) return;

    Proceso_t *temp = cola;

    do
    {
        if (temp != actual)
        {
            temp->waiting_time += tiempo;
        }

        temp = temp->siguiente;

    } while (temp != cola);
}
