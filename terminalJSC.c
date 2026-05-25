#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nodosCirculares.h"
#include "catJSC.h"
#include "rmJSC.h"
#include "cpJSC.h"
#include "infoJSC.h"
#include "practica5.h"
#include "procesos.h"
#include "nodos.h"

#include "knn.h"
#include "memoriaJSC.h"

int main()
{
    char str[1024] = "";
    char delimiter[] = " ";
    char *tokens[1024];
    int i_tokens, token_numeros, exit = 0;
    Proceso_t *cabeza = NULL;
    Bloque *memoriaPrincipal = NULL;


    printf("\nPara mas informacion, teclea infoJSC y lee bien\n\n");
    
    int mkprocess_flag, lstprocss_flag, fcfs_flag, sjf_flag, rr_flag, my_kill_flag;
    int setmem_flag, alloc_flag, free_flag, mstatus_flag, compact_flag;

    while (exit != 1)
    {
        
        mkprocess_flag = lstprocss_flag = fcfs_flag = sjf_flag = rr_flag = my_kill_flag = 0;
        setmem_flag = alloc_flag = free_flag = mstatus_flag = compact_flag = 0;
        i_tokens = 0; token_numeros = 0;

        printf("\nLeyendo: ");
        if (!fgets(str, 1024, stdin)) break;

        if (token_numeros == 1) {
            if (strcmp(str, "exit\n") == 0) exit = 1;
            if (strcmp(str, "infoJSC\n") == 0) infoJSC();
        }

        char *token = strtok(str, delimiter);
        while (token) {
            tokens[i_tokens++] = token;
            if (strcmp(token, "mkprocess") == 0) mkprocess_flag = 1;
            if (strcmp(token, "lstprocss\n") == 0) lstprocss_flag = 1;
            if (strcmp(token, "FCFS\n") == 0) fcfs_flag = 1;
            if (strcmp(token, "SJF\n") == 0) sjf_flag = 1;
            if (strcmp(token, "RR\n") == 0) rr_flag = 1;
            if (strcmp(token, "my_kill") == 0) my_kill_flag = 1;
            if (strcmp(token, "setmem") == 0) setmem_flag = 1;
            if (strcmp(token, "alloc") == 0) alloc_flag = 1;
            if (strcmp(token, "free") == 0) free_flag = 1;
            if (strcmp(token, "mstatus\n") == 0) mstatus_flag = 1;
            if (strcmp(token, "compact\n") == 0) compact_flag = 1;
            if (strcmp(token, "exit\n") == 0) exit = 1;
            token = strtok(NULL, delimiter);
            token_numeros++;
        }

       
        if (mkprocess_flag && token_numeros == 4) {
            char *id = tokens[1];
            int burst = atoi(tokens[2]);
            int tam = atoi(tokens[3]);
            if (buscar_id(cabeza, id) == 0) {
                mkprocess(&cabeza, id, burst, tam);
            }
        }

        if (my_kill_flag && token_numeros == 2) {
            tokens[1][strcspn(tokens[1], "\n")] = '\0';
            if (memoriaPrincipal) freeJSC(memoriaPrincipal, tokens[1]);
            eliminar_proceso(&cabeza, tokens[1]);
        }

        
        if (setmem_flag && token_numeros == 2) {
            memoriaPrincipal = crear_bloque(0, atoi(tokens[1]));
        }

        if (alloc_flag && token_numeros == 3) {
            tokens[2][strcspn(tokens[2], "\n")] = '\0';
            Proceso_t* p = cabeza; int enc = 0;
            if (p) {
                do {
                    if (strcmp(p->id, tokens[1]) == 0) { enc = 1; break; }
                    p = p->siguiente;
                } while (p != cabeza);
            }
            if (enc && memoriaPrincipal) {
                allocJSC(memoriaPrincipal, p->id, p->tamano_memoria, tokens[2]);
                strcpy(p->estado, "READY"); 
            }
        }

        if (fcfs_flag) fcfsJSC(&cabeza);
        if (sjf_flag) sjfJSC(&cabeza, 0);
        if (rr_flag) rrJSC(&cabeza, 10);
        if (lstprocss_flag) lstprocss(cabeza);
        if (mstatus_flag) mstatusJSC(memoriaPrincipal);
        if (compact_flag) compactJSC(memoriaPrincipal);

        if (token_numeros == 1) {
            if (strcmp(str, "exit\n") == 0) exit = 1;
            if (strcmp(str, "infoJSC\n") == 0) infoJSC();
        }
    }
    return 0;
}