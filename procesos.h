#ifndef PROCESOS_H
#define PROCESOS_H

#include "nodosCirculares.h"
#include "memoriaJSC.h"

void fcfsJSC(Proceso_t **cola, Bloque *memoria);
void sjfJSC(Proceso_t **cola, Bloque *memoria);
void rrJSC(Proceso_t **cola, int quantum, Bloque *memoria);
void aumentar_waiting(Proceso_t *cola, Proceso_t *actual, int tiempo);
int contar_ready(Proceso_t *cola);

#endif