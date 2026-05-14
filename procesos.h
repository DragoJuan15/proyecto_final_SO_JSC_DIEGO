#ifndef PROCESOS_H
#define PROCESOS_H

#include "nodosCirculares.h"

void fcfsJSC(Proceso_t **cola);
void sjfJSC(Proceso_t **cola, int preemptivo);
void rrJSC(Proceso_t **cola, int quantum);
void aumentar_waiting(Proceso_t *cola, Proceso_t *actual, int tiempo);

#endif