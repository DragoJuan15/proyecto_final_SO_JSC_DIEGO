#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "procesos.h"

// función para verificar si hay procesos READY
int contar_ready(Proceso_t *cola) {
    if (cola == NULL) return 0;
    int contador = 0;
    Proceso_t *temp = cola;
    do {
        if (strcmp(temp->estado, "READY") == 0) contador++;
        temp = temp->siguiente;
    } while (temp != cola);
    return contador;
}

void fcfsJSC(Proceso_t **cola) {
    if (*cola == NULL || contar_ready(*cola) == 0) {
        printf("No hay procesos en estado READY (usa alloc primero).\n");
        return;
    }

    int turn = 0;
    float suma_wait = 0, suma_turn = 0;
    int total_procesos = 0;

    Proceso_t *actual = *cola;
    
    // ejecutamos los que estén READY
    // hasta que no queden procesos READY en la cola
    while (contar_ready(*cola) > 0) {
        if (strcmp((*cola)->estado, "READY") == 0) {
            int wait = turn;
            turn += (*cola)->burst_time;
            
            (*cola)->waiting_time = wait;
            (*cola)->turnaround_time = turn;

            suma_wait += wait;
            suma_turn += turn;
            total_procesos++;

            printf("[EXEC FCFS] ID: %s | Wait: %d | Turn: %d | Burst: %d\n", 
                    (*cola)->id, (*cola)->waiting_time, (*cola)->turnaround_time, (*cola)->burst_time);
            
            eliminar_proceso(cola, (*cola)->id);
        } else {
            *cola = (*cola)->siguiente; // Brincar los "NEW"
        }
    }

    if (total_procesos > 0) {
        printf("\n--- PROMEDIOS FCFS ---\n");
        printf("Avg Waiting Time: %.2f\n", suma_wait / total_procesos);
        printf("Avg Turnaround Time: %.2f\n", suma_turn / total_procesos);
    }
}

void sjfJSC(Proceso_t **cola, int preemptivo) {
    if (*cola == NULL || contar_ready(*cola) == 0) {
        printf("No hay procesos READY para SJF.\n");
        return;
    }

    int turn = 0;
    float suma_wait = 0, suma_turn = 0;
    int total_procesos = 0;

    while (contar_ready(*cola) > 0) {
        Proceso_t *temp = *cola;
        Proceso_t *menor = NULL;

        // Buscar el proceso READY con el burst más corto
        do {
            if (strcmp(temp->estado, "READY") == 0) {
                if (menor == NULL || temp->burst_time < menor->burst_time) {
                    menor = temp;
                }
            }
            temp = temp->siguiente;
        } while (temp != *cola);

        if (menor) {
            int wait = turn;
            turn += menor->burst_time;
            menor->waiting_time = wait;
            menor->turnaround_time = turn;

            suma_wait += wait;
            suma_turn += turn;
            total_procesos++;

            printf("[EXEC SJF] ID: %s | Wait: %d | Turn: %d | Burst: %d\n", 
                    menor->id, menor->waiting_time, menor->turnaround_time, menor->burst_time);
            
            eliminar_proceso(cola, menor->id);
        }
    }
    
    if (total_procesos > 0) {
        printf("\n--- PROMEDIOS SJF ---\n");
        printf("Avg Waiting Time: %.2f\n", suma_wait / total_procesos);
        printf("Avg Turnaround Time: %.2f\n", suma_turn / total_procesos);
    }
}

void rrJSC(Proceso_t **cola, int quantum) {
    if (*cola == NULL || contar_ready(*cola) == 0) {
        printf("No hay procesos READY para RR.\n");
        return;
    }

    int tiempo_global = 0;
    float suma_wait = 0, suma_turn = 0;
    int total_finalizados = 0;
    int procesos_iniciales = contar_ready(*cola);

    while (contar_ready(*cola) > 0) {
        if (strcmp((*cola)->estado, "READY") == 0) {
            if ((*cola)->burst_time > quantum) {
                tiempo_global += quantum;
                (*cola)->burst_time -= quantum;
                
                printf("[RR QUANTUM] ID: %s | Burst restante: %d | Tiempo: %d\n", 
                        (*cola)->id, (*cola)->burst_time, tiempo_global);
                
                aumentar_waiting(*cola, *cola, quantum);
                *cola = (*cola)->siguiente;
            } else {
                int residuo = (*cola)->burst_time;
                tiempo_global += residuo;
                
                // El turnaround es el tiempo global cuando termina
                int turn = tiempo_global;
                int wait = turn - ((*cola)->turnaround_time + residuo); // Ajuste de lógica de wait
               
                suma_turn += turn;
                suma_wait += (*cola)->waiting_time;
                total_finalizados++;

                printf("[RR FINISH] ID: %s | Wait: %d | Turn: %d\n", 
                        (*cola)->id, (*cola)->waiting_time, turn);

                aumentar_waiting(*cola, *cola, residuo);
                eliminar_proceso(cola, (*cola)->id);
            }
        } else {
            *cola = (*cola)->siguiente; // Ignorar los que no están READY
        }
    }

    if (total_finalizados > 0) {
        printf("\n--- PROMEDIOS RR ---\n");
        printf("Avg Waiting Time: %.2f\n", suma_wait / total_finalizados);
        printf("Avg Turnaround Time: %.2f\n", suma_turn / total_finalizados);
    }
}


void aumentar_waiting(Proceso_t *cola, Proceso_t *actual, int tiempo) {
    if (cola == NULL) return;
    Proceso_t *temp = cola;
    do {
        if (temp != actual && strcmp(temp->estado, "READY") == 0) {
            temp->waiting_time += tiempo;
        }
        temp = temp->siguiente;
    } while (temp != cola);
}