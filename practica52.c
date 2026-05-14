#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "nodos.h"

#define read_end 0
#define write_end 1

void terminalJSC(char *tokens[], int n)
{
    Comando_t *lista = NULL;

    if (n <= 1)
    {
        printf("terminalJSC: no se ingresaron comandos.\n");
        return;
    }

    char buffer[256] = "";
    int hay_algo = 0;

    for (int i = 1; i < n; i++)
    {
        tokens[i][strcspn(tokens[i], "\n")] = '\0';

        if (strcmp(tokens[i], "|") == 0)
        {
            if (hay_algo)
                insertar_comando(&lista, buffer);
            buffer[0] = '\0';
            hay_algo = 0;
        }
        else
        {
            if (hay_algo)
                strcat(buffer, " ");
            strcat(buffer, tokens[i]);
            hay_algo = 1;
        }
    }
    if (hay_algo){
        insertar_comando(&lista, buffer);
    }

    printf("\n--- LISTA DESDE TOKENS ---\n");
    imprimir_lista(lista);
    printf("--------------------------\n\n");

    int num_cmd = 0;
    Comando_t *temp = lista;
    while (temp != NULL)
    {
        num_cmd++;
        temp = temp->siguiente;
    }

    int pipa[64][2];
    pid_t pid[64];

    for (int i = 0; i < num_cmd - 1; i++)
    {
        if (pipe(pipa[i]) == -1)
        {
            perror("pipe");
            liberar_lista(&lista);
            return;
        }
    }

    Comando_t *nodo = lista;

    for (int i = 0; i < num_cmd; i++)
    {
        pid[i] = fork();

        if (pid[i] < 0)
        {
            perror("fork");
            liberar_lista(&lista);
            return;
        }

        if (pid[i] == 0)
        {
            if (i > 0)
                dup2(pipa[i - 1][read_end], STDIN_FILENO);

            if (i < num_cmd - 1)
                dup2(pipa[i][write_end], STDOUT_FILENO);

            for (int j = 0; j < num_cmd - 1; j++)
            {
                close(pipa[j][read_end]);
                close(pipa[j][write_end]);
            }

            char *argv[64];
            int argc = 0;
            char *tok = strtok(nodo->comando, " ");
            while (tok != NULL)
            {
                argv[argc++] = tok;
                tok = strtok(NULL, " ");
            }
            argv[argc] = NULL;

            execvp(argv[0], argv);
            perror("execvp");
            exit(1);
        }

        nodo = nodo->siguiente;
    }

    for (int i = 0; i < num_cmd - 1; i++)
    {
        close(pipa[i][read_end]);
        close(pipa[i][write_end]);
    }

    for (int i = 0; i < num_cmd; i++)
        waitpid(pid[i], NULL, 0);

    liberar_lista(&lista);
}