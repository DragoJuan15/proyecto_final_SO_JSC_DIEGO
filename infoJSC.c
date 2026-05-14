#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "catJSC.h"

void infoJSC()
{
    printf("\ncpJSC -> es copiar, primero va origen y luego destino");
    printf("\ncatJSC -> crea o abre archivos");
    printf("\nrmJSC -> borra archivos");
    printf("\nterminalJSC -> escribe como si fuera bash sin problemas");
    printf("\nprocesosJSC -> trabaja con procesos con burst time de 3 formas distintas");
    printf("\nexit -> sale del programa");
    printf("\n\n----------------------------------------------------------------------------------------------------------------\n\n");
}
