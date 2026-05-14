#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>

typedef struct Comando
{
    char comando[256];
    struct Comando *siguiente;
} Comando_t;

void insertar_comando(Comando_t **cabeza, char texto[])
{
    Comando_t *nuevo = (Comando_t *)malloc(sizeof(Comando_t));
    strcpy(nuevo->comando, texto);
    nuevo->siguiente = NULL;

    if (*cabeza == NULL)
    {
        *cabeza = nuevo;
    }
    else
    {
        Comando_t *temp = *cabeza;
        while (temp->siguiente != NULL)
        {
            temp = temp->siguiente;
        }
        temp->siguiente = nuevo;
    }
}

void imprimir_lista(Comando_t *cabeza)
{
    int i = 0;
    while (cabeza != NULL)
    {
        printf("Comando %d: [%s]\n", i, cabeza->comando);
        cabeza = cabeza->siguiente;
        i++;
    }
}

void liberar_lista(Comando_t **cabeza)
{
    Comando_t *temp;
    while (*cabeza != NULL)
    {
        temp = *cabeza;
        *cabeza = (*cabeza)->siguiente;
        free(temp);
    }
}

void parsear_comandos(char input[], Comando_t **lista)
{
    char *token = strtok(input, "|");

    while (token)
    {
        while (*token == ' ')
        {
            token++;
        }

        token[strcspn(token, "\n")] = '\0';

        insertar_comando(lista, token);

        token = strtok(NULL, "|");
    }
}
/*
int main()
{
    Comando_t *lista = NULL;

    char input[256];

    printf("Ingresa comando tipo bash:\n");
    fgets(input, sizeof(input), stdin);

    parsear_comandos(input, &lista);

    printf("\n--- LISTA GENERADA ---\n");
    imprimir_lista(lista);

    liberar_lista(&lista);

    return 0;
}
*/
