#ifndef NODOS_CIRCULARES_H
#define NODOS_CIRCULARES_H

typedef struct Proceso
{
    char id [50];
    int burst_time;
    int waiting_time;
    int turnaround_time;
    struct Proceso *siguiente;
} Proceso_t;

void mkprocess(Proceso_t **cola, char id [], int burst);
void eliminar_proceso(Proceso_t **cola, char id[]);
void lstprocss(Proceso_t *cola);
void liberar_cola(Proceso_t **cola);
int buscar_id(Proceso_t *cola, char id[]);

#endif