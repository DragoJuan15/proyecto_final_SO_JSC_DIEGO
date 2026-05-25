#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "catJSC.h"

void infoJSC()
{
    printf("\ncpJSC <origen> <destino> -> es copiar archivos");
    printf("\ncatJSC <archivo> -> crea o abre archivos");
    printf("\nrmJSC <archivo> -> borra archivos");
    printf("\nterminalJSC <comando> -> escribe como si fuera bash sin problemas (soporta |)");
    printf("\nsetmem <tamaño> -> inicializa el tamaño de la memoria principal");
    printf("\nalloc <id> <estrategia> -> asigna memoria (estrategias: FF, BF, WF)");
    printf("\nfree <id> -> libera la memoria de un proceso");
    printf("\nmstatus -> muestra el mapa de memoria con bases y tamaños");
    printf("\ncompact -> realiza la compactacion de la memoria");
    printf("\nmkprocess <id> <burst> <bloques> -> crea un proceso en estado NEW");
    printf("\nlstprocss -> lista procesos con estado y bloques que ocupa");
    printf("\nmy_kill <id> -> elimina un proceso de la lista y de memoria");
    printf("\nFCFS -> ejecuta planificacion First-Come, First-Served");
    printf("\nSJF -> ejecuta planificacion Shortest-Job-First");
    printf("\nRR -> ejecuta planificacion Round-Robin (Quantum 10)");
    printf("\nexit -> sale del programa");
    printf("\n\n----------------------------------------------------------------------------------------------------------------\n\n");
}