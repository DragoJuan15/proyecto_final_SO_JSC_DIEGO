#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "procesos.h"
#include "memoriaJSC.h"

//verifica cuántos procesos están en memoria listos para ejecución
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

void fcfsJSC(Proceso_t **cola, Bloque *memoria) {
    if (*cola == NULL || contar_ready(*cola) == 0) {
        printf("No hay procesos en estado READY (usa alloc primero).\n");
        return;
    }

    int tiempo_actual = 0;
    float suma_wait = 0, suma_turn = 0;
    int total_ejecutados = 0;
    Proceso_t *actual = *cola;
    Proceso_t *inicio = *cola;

    do {
        //solo si el proceso está en memoria (READY)
        if (strcmp(actual->estado, "READY") == 0) {
            actual->waiting_time = tiempo_actual;
            actual->turnaround_time = actual->waiting_time + actual->burst_time;
            tiempo_actual = actual->turnaround_time;

            suma_wait += actual->waiting_time;
            suma_turn += actual->turnaround_time;
            total_ejecutados++;

            printf("[EXEC FCFS] ID: %s | Entrada: %d | Salida: %d | Wait: %d | Turn: %d\n", 
                    actual->id, actual->waiting_time, actual->turnaround_time, 
                    actual->waiting_time, actual->turnaround_time);
            
            strcpy(actual->estado, "TERMINATED");
            if (memoria != NULL) freeJSC(memoria, actual->id);
        }
        actual = actual->siguiente;
    } while (actual != inicio);

    if (total_ejecutados > 0) {
        printf("\n--- PROMEDIOS FCFS ---\n");
        printf("Avg Waiting Time: %.2f\n", suma_wait / total_ejecutados);
        printf("Avg Turnaround Time: %.2f\n", suma_turn / total_ejecutados);
    }
}

void sjfJSC(Proceso_t **cola, Bloque *memoria) {
    if (*cola == NULL || contar_ready(*cola) == 0) {
        printf("No hay procesos READY para SJF.\n");
        return;
    }

    int tiempo_actual = 0;
    float suma_wait = 0, suma_turn = 0;
    int total_ejecutados = 0;

    while (contar_ready(*cola) > 0) {
        Proceso_t *temp = *cola;
        Proceso_t *menor = NULL;

        do {
            if (strcmp(temp->estado, "READY") == 0) {
                if (menor == NULL || temp->burst_time < menor->burst_time) {
                    menor = temp;
                }
            }
            temp = temp->siguiente;
        } while (temp != *cola);

        if (menor) {
            menor->waiting_time = tiempo_actual;
            menor->turnaround_time = menor->waiting_time + menor->burst_time;
            tiempo_actual = menor->turnaround_time;

            suma_wait += menor->waiting_time;
            suma_turn += menor->turnaround_time;
            total_ejecutados++;

            printf("[EXEC SJF] ID: %s | Wait: %d | Turn: %d | Burst: %d\n", 
                    menor->id, menor->waiting_time, menor->turnaround_time, menor->burst_time);
            
            strcpy(menor->estado, "TERMINATED");
            if (memoria != NULL) freeJSC(memoria, menor->id);
        }
    }
    
    if (total_ejecutados > 0) {
        printf("\n--- PROMEDIOS SJF ---\n");
        printf("Avg Waiting Time: %.2f\n", suma_wait / total_ejecutados);
        printf("Avg Turnaround Time: %.2f\n", suma_turn / total_ejecutados);
    }
}

void rrJSC(Proceso_t **cola, int quantum, Bloque *memoria) {
    if (*cola == NULL || contar_ready(*cola) == 0) {
        printf("No hay procesos READY para RR.\n");
        return;
    }

    int tiempo_global = 0;
    float suma_wait = 0, suma_turn = 0;
    int total_finalizados = 0;

    while (contar_ready(*cola) > 0) {
        //Ignorar cualquier nodo que noeste READY 
        if (strcmp((*cola)->estado, "READY") == 0) {
            if ((*cola)->burst_time > quantum) {
                tiempo_global += quantum;
                (*cola)->burst_time -= quantum;
                
                printf("[RR QUANTUM] ID: %s | Burst restante: %d | Tiempo Global: %d\n", 
                        (*cola)->id, (*cola)->burst_time, tiempo_global);
                
                aumentar_waiting(*cola, *cola, quantum);
            } else {
                int residuo = (*cola)->burst_time;
                tiempo_global += residuo;
                
                int t_final = tiempo_global;
                suma_turn += t_final;
                suma_wait += (*cola)->waiting_time;
                total_finalizados++;

                printf("[RR FINISH] ID: %s | Wait Final: %d | Turn Final: %d\n", 
                        (*cola)->id, (*cola)->waiting_time, t_final);

                aumentar_waiting(*cola, *cola, residuo);
                
                strcpy((*cola)->estado, "TERMINATED");
                if (memoria != NULL) freeJSC(memoria, (*cola)->id);
            }
        }
       
        *cola = (*cola)->siguiente; 
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