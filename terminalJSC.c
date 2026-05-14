#include <stdio.h>
#include <string.h>
#include "catJSC.h"
#include "rmJSC.h"
#include "cpJSC.h"
#include "infoJSC.h"
#include "practica5.h"
#include "procesos.h"
#include "nodos.h"
#include "nodosCirculares.h"
#include "knn.h"

int main()
{
    char str[1024] = "";
    char delimiter[] = " ";
    char *token;
    char *palabra_clave;
    char *palabra_clave2;
    int catJSC_flag = 0;
    int rmJSC_flag = 0;
    int cpJSC_flag = 0;
    int terminalJSC_flag = 0;
    int mkprocess_flag = 0;
    int lstprocss_flag = 0;
    int fcfsJSC_flag = 0;
    int sjfJSC_flag = 0;
    int sjfPJSC_flag = 0;
    int rrJSC_flag = 0;
    int knn_flag = 0;
    int token_numeros = 0;
    int exit = 0;
    char *tokens[1024];
    int i_tokens = 0;
    Proceso_t *cabeza = NULL;

    printf("\nPara mas informacion, teclea infoJSC y lee bien\n\n");

    while (exit != 1)
    {
        printf("Leyendo: ");
        fgets(str, 100, stdin);

        token = strtok(str, delimiter);

        while (token)
        {
            tokens[i_tokens++] = token;

            if (strcmp(token, "catJSC") == 0)
            {
                catJSC_flag = 1;
            }
            if (strcmp(token, "rmJSC") == 0)
            {
                rmJSC_flag = 1;
            }
            if (strcmp(token, "cpJSC") == 0)
            {
                cpJSC_flag = 1;
            }
            if (strcmp(token, "terminalJSC") == 0)
            {
                terminalJSC_flag = 1;
            }
            if (strcmp(token, "mkprocess") == 0)
            {
                mkprocess_flag = 1;
            }
            if (strcmp(token, "lstprocss\n") == 0)
            {
                lstprocss_flag = 1;
            }
            if (strcmp(token, "fcfsJSC\n") == 0)
            {
                fcfsJSC_flag = 1;
            }
            if (strcmp(token, "sjfJSC\n") == 0)
            {
                sjfJSC_flag = 1;
            }
            if (strcmp(token, "sjfPJSC\n") == 0)
            {
                sjfPJSC_flag = 1;
            }
            if (strcmp(token, "rrJSC\n") == 0)
            {
                rrJSC_flag = 1;
            }
            if (strcmp(token, "knnJSC\n") == 0)
            {
                knn_flag = 1;
            }
            if (strcmp(token, "exit") == 0)
            {
                exit = 1;
            }
            palabra_clave2 = palabra_clave;
            palabra_clave = token;
            token = strtok(NULL, delimiter);
            token_numeros++;
        }

        if (catJSC_flag == 1 && token_numeros == 2)
        {
            printf("Usastes cat\n");
            palabra_clave[strlen(palabra_clave) - 1] = '\0';
            catJSC(palabra_clave);
        }
        if (rmJSC_flag == 1 && token_numeros == 2)
        {
            printf("Usastes rm, %s\n", palabra_clave);
            palabra_clave[strlen(palabra_clave) - 1] = '\0';
            rmJSC(palabra_clave);
        }
        if (cpJSC_flag == 1 && token_numeros == 3)
        {
            printf("Usastes cp, %s\n", palabra_clave);
            palabra_clave[strlen(palabra_clave) - 1] = '\0';
            cpJSC(palabra_clave2, palabra_clave);
        }
        if (terminalJSC_flag == 1)
        {
            terminalJSC(tokens, i_tokens);
        }
        if (mkprocess_flag == 1 && token_numeros == 3)
        {
            if (buscar_id(cabeza, palabra_clave2) == 0)
            {
                int burst = atoi(palabra_clave);
                printf("\nCrear Proceso \n");
                mkprocess(&cabeza, palabra_clave2, burst);
            }
        }
        if (lstprocss_flag == 1)
        {
            printf("\nVer Procesos\n");
            lstprocss(cabeza);
        }
        if (fcfsJSC_flag == 1)
        {
            printf("\nFirst Come First Served\n");
            fcfsJSC(&cabeza);
        }
        if (sjfJSC_flag == 1)
        {
            printf("\nShortest Job First\n");
            sjfJSC(&cabeza, 0);
        }
        if (sjfPJSC_flag == 1)
        {
            printf("\nShortest Job First P\n");
            sjfJSC(&cabeza, 1);
        }
        if (rrJSC_flag == 1)
        {
            printf("\nRound Robin\n");
            rrJSC(&cabeza, 10);
        }
        if (knn_flag == 1 && token_numeros == 3)
        {
            printf("\nKNN\n");
            int pal1 = atoi(palabra_clave);
            int pal2 = atoi(palabra_clave2);
            knnJSC("trn_set.csv", "tst_set.csv", pal2, pal1);
        }

        catJSC_flag = 0;
        rmJSC_flag = 0;
        cpJSC_flag = 0;
        terminalJSC_flag = 0;
        mkprocess_flag = 0;
        lstprocss_flag = 0;
        fcfsJSC_flag = 0;
        sjfJSC_flag = 0;
        sjfPJSC_flag = 0;
        rrJSC_flag = 0;
        knn_flag = 0;

        if (token_numeros == 1)
        {
            if (strcmp(str, "exit\n") == 0)
            {
                exit = 1;
                printf("\nSaliendo\n\n...\n\n");
            }
            if (strcmp(str, "infoJSC\n") == 0)
            {
                printf("\nInformacion de la terminal\n");
                infoJSC();
            }
        }
        token_numeros = 0;
        i_tokens = 0;
        token = "";
        palabra_clave = "";
    }
    return 0;
}