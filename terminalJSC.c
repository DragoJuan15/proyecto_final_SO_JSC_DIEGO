#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "catJSC.h"
#include "rmJSC.h"
#include "cpJSC.h"
#include "infoJSC.h"
#include "practica5.h"
#include "procesos.h"
#include "nodos.h"
#include "nodosCirculares.h"
#include "knn.h"
#include "memoriaJSC.h"

int main()
{
    char str[1024] = "";
    char delimiter[] = " ";
    char *token;
    char *palabra_clave;
    char *palabra_clave2;
    int catJSC_flag = 0, rmJSC_flag = 0, cpJSC_flag = 0, terminalJSC_flag = 0;
    int mkprocess_flag = 0, lstprocss_flag = 0, fcfsJSC_flag = 0, sjfJSC_flag = 0;
    int sjfPJSC_flag = 0, rrJSC_flag = 0, knn_flag = 0;
    
    //banderas de memoria
    int setmem_flag = 0, alloc_flag = 0, free_flag = 0, mstatus_flag = 0, compact_flag = 0;
    
    int token_numeros = 0;
    int exit = 0;
    char *tokens[1024];
    int i_tokens = 0;
    Proceso_t *cabeza = NULL;
    Bloque *memoriaPrincipal = NULL;

    printf("\nPara mas informacion, teclea infoJSC y lee bien\n\n");

    while (exit != 1)
    {
        printf("Leyendo: ");
        fgets(str, 100, stdin);

        token = strtok(str, delimiter);

        while (token)
        {
            tokens[i_tokens++] = token;

            if (strcmp(token, "catJSC") == 0) catJSC_flag = 1;
            if (strcmp(token, "rmJSC") == 0) rmJSC_flag = 1;
            if (strcmp(token, "cpJSC") == 0) cpJSC_flag = 1;
            if (strcmp(token, "terminalJSC") == 0) terminalJSC_flag = 1;
            if (strcmp(token, "mkprocess") == 0) mkprocess_flag = 1;
            if (strcmp(token, "lstprocss\n") == 0) lstprocss_flag = 1;
            if (strcmp(token, "fcfsJSC\n") == 0) fcfsJSC_flag = 1;
            if (strcmp(token, "sjfJSC\n") == 0) sjfJSC_flag = 1;
            if (strcmp(token, "sjfPJSC\n") == 0) sjfPJSC_flag = 1;
            if (strcmp(token, "rrJSC\n") == 0) rrJSC_flag = 1;
            if (strcmp(token, "knnJSC") == 0) knn_flag = 1;
            if (strcmp(token, "exit") == 0) exit = 1;
            
            //comandos memoria
            if (strcmp(token, "setmem") == 0) setmem_flag = 1;
            if (strcmp(token, "alloc") == 0) alloc_flag = 1;
            if (strcmp(token, "free") == 0) free_flag = 1;
            if (strcmp(token, "mstatus\n") == 0) mstatus_flag = 1;
            if (strcmp(token, "compact\n") == 0) compact_flag = 1;

            palabra_clave2 = palabra_clave;
            palabra_clave = token;
            token = strtok(NULL, delimiter);
            token_numeros++;
        }

        // logica de memoria
        if (setmem_flag == 1 && token_numeros == 2) {
            int tam = atoi(palabra_clave);
            memoriaPrincipal = crear_bloque(0, tam);
            printf("Memoria inicializada con %d unidades.\n", tam);
        }

        if (alloc_flag == 1 && token_numeros == 3) {
            palabra_clave[strlen(palabra_clave) - 1] = '\0'; //limpiar estrategia
            
            // BUSQUEDA CIRCULAR
            Proceso_t* p = cabeza; 
            int encontrado = 0;
            if (p != NULL) {
                do {
                    if (strcmp(p->id, palabra_clave2) == 0) {
                        encontrado = 1;
                        break;
                    }
                    p = p->siguiente;
                } while (p != cabeza);
            }

            if(encontrado && memoriaPrincipal) {
                allocJSC(memoriaPrincipal, p->id, p->burst_time, palabra_clave);
            } else {
                printf("Error: Proceso no encontrado o memoria no iniciada.\n");
            }
        }

        if (free_flag == 1 && token_numeros == 2) {
            palabra_clave[strlen(palabra_clave) - 1] = '\0';
            if(memoriaPrincipal) freeJSC(memoriaPrincipal, palabra_clave);
        }

        if (mstatus_flag == 1) mstatusJSC(memoriaPrincipal);
        if (compact_flag == 1) compactJSC(memoriaPrincipal);

        if (catJSC_flag == 1 && token_numeros == 2) {
            palabra_clave[strlen(palabra_clave) - 1] = '\0';
            catJSC(palabra_clave);
        }
        if (rmJSC_flag == 1 && token_numeros == 2) {
            palabra_clave[strlen(palabra_clave) - 1] = '\0';
            rmJSC(palabra_clave);
        }
        if (cpJSC_flag == 1 && token_numeros == 3) {
            palabra_clave[strlen(palabra_clave) - 1] = '\0';
            cpJSC(palabra_clave2, palabra_clave);
        }
        if (terminalJSC_flag == 1) terminalJSC(tokens, i_tokens);
        
        if (mkprocess_flag == 1 && token_numeros == 3) {
            if (buscar_id(cabeza, palabra_clave2) == 0) {
                int burst = atoi(palabra_clave);
                mkprocess(&cabeza, palabra_clave2, burst);
            }
        }

        if (lstprocss_flag == 1) lstprocss(cabeza);
        if (fcfsJSC_flag == 1) fcfsJSC(&cabeza);
        if (sjfJSC_flag == 1) sjfJSC(&cabeza, 0);
        if (sjfPJSC_flag == 1) sjfJSC(&cabeza, 1);
        if (rrJSC_flag == 1) rrJSC(&cabeza, 10);
        
        if (knn_flag == 1 && token_numeros == 3) {
            int pal1 = atoi(palabra_clave);
            int pal2 = atoi(palabra_clave2);
            knnJSC("trn_set.csv","tst_set.csv",pal2, pal1);
        }

        // Reset flags
        catJSC_flag = rmJSC_flag = cpJSC_flag = terminalJSC_flag = 0;
        mkprocess_flag = lstprocss_flag = fcfsJSC_flag = sjfJSC_flag = 0;
        sjfPJSC_flag = rrJSC_flag = knn_flag = 0;
        setmem_flag = alloc_flag = free_flag = mstatus_flag = compact_flag = 0;

        if (token_numeros == 1) {
            if (strcmp(str, "exit\n") == 0) exit = 1;
            if (strcmp(str, "infoJSC\n") == 0) infoJSC();
        }
        
        token_numeros = 0;
        i_tokens = 0;
        token = "";
        palabra_clave = "";
    }
    return 0;
}