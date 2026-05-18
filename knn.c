#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "knn.h"

typedef struct Nodo
{
    double x;
    double y;
    char clase[100];
    struct Nodo *siguiente;
} Nodo_t;

typedef struct Distancia
{
    double distancia;
    char clase[100];
} Distancia_t;

typedef struct Hilo
{
    Nodo_t *train;
    Nodo_t *test_inicio;
    int inicio;
    int fin;
    int k;
    char **resultados;
} Hilo_t;

Nodo_t *crear_nodo(double x, double y, char clase[])
{
    Nodo_t *nuevo = (Nodo_t *)malloc(sizeof(Nodo_t));

    nuevo->x = x;
    nuevo->y = y;
    strcpy(nuevo->clase, clase);
    nuevo->siguiente = NULL;

    return nuevo;
}

void insertar_nodo(Nodo_t **cabeza, double x, double y, char clase[])
{
    Nodo_t *nuevo = crear_nodo(x, y, clase);

    if (*cabeza == NULL)
    {
        *cabeza = nuevo;
        return;
    }

    Nodo_t *aux = *cabeza;

    while (aux->siguiente != NULL)
    {
        aux = aux->siguiente;
    }

    aux->siguiente = nuevo;
}

int contar_nodos(Nodo_t *cabeza)
{
    int contador = 0;

    while (cabeza != NULL)
    {
        contador++;
        cabeza = cabeza->siguiente;
    }

    return contador;
}

Nodo_t *obtener_nodo(Nodo_t *cabeza, int indice)
{
    int i = 0;

    while (cabeza != NULL)
    {
        if (i == indice)
        {
            return cabeza;
        }

        cabeza = cabeza->siguiente;
        i++;
    }

    return NULL;
}

Nodo_t *leer_csv(char archivo[])
{
    FILE *fd = fopen(archivo, "r");

    if (fd == NULL)
    {
        printf("Error archivo\n");
        return NULL;
    }

    Nodo_t *cabeza = NULL;

    char linea[256];

    while (fgets(linea, sizeof(linea), fd))
    {
        double x;
        double y;

        linea[strcspn(linea, "\n")] = 0;

        sscanf(linea, "%lf,%lf", &x, &y);

        insertar_nodo(&cabeza, x, y, "");
    }

    fclose(fd);

    return cabeza;
}

double calcular_distancia(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void ordenar_distancias(Distancia_t distancias[], int n)
{
    int i;
    int j;

    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (distancias[j].distancia > distancias[j + 1].distancia)
            {
                Distancia_t aux = distancias[j];
                distancias[j] = distancias[j + 1];
                distancias[j + 1] = aux;
            }
        }
    }
}

char *clasificar(Nodo_t *train, Nodo_t *test, int k)
{
    int total_train = contar_nodos(train);

    Distancia_t *distancias = (Distancia_t *)malloc(sizeof(Distancia_t) * total_train);

    Nodo_t *aux = train;

    int i = 0;

    while (aux != NULL)
    {
        distancias[i].distancia = calcular_distancia(
            test->x,
            test->y,
            aux->x,
            aux->y);

        strcpy(distancias[i].clase, aux->clase);

        aux = aux->siguiente;
        i++;
    }

    ordenar_distancias(distancias, total_train);

    int contadorA = 0;
    int contadorB = 0;

    for (i = 0; i < k; i++)
    {
        if (strcmp(distancias[i].clase, "A") == 0)
        {
            contadorA++;
        }
        else
        {
            contadorB++;
        }
    }

    free(distancias);

    char *resultado = (char *)malloc(100);

    if (contadorA > contadorB)
    {
        strcpy(resultado, "A");
    }
    else
    {
        strcpy(resultado, "B");
    }

    return resultado;
}

void *knn_hilo(void *arg)
{
    Hilo_t *datos = (Hilo_t *)arg;

    int i;

    for (i = datos->inicio; i < datos->fin; i++)
    {
        Nodo_t *test = obtener_nodo(datos->test_inicio, i);

        datos->resultados[i] = clasificar(datos->train, test, datos->k);
    }

    return NULL;
}

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos)
{
    Nodo_t *train = leer_csv(entrenamiento);
    Nodo_t *test = leer_csv(prueba);

    if (train == NULL || test == NULL)
    {
        return;
    }

    int total_test = contar_nodos(test);

    char **resultados = (char **)malloc(sizeof(char *) * total_test);

    pthread_t threads[hilos];

    Hilo_t datos[hilos];

    int bloque = total_test / hilos;

    int inicio = 0;

    int i;

    for (i = 0; i < hilos; i++)
    {
        datos[i].train = train;
        datos[i].test_inicio = test;
        datos[i].inicio = inicio;

        if (i == hilos - 1)
        {
            datos[i].fin = total_test;
        }
        else
        {
            datos[i].fin = inicio + bloque;
        }

        datos[i].k = k;
        datos[i].resultados = resultados;

        pthread_create(&threads[i], NULL, knn_hilo, (void *)&datos[i]);

        inicio = datos[i].fin;
    }

    for (i = 0; i < hilos; i++)
    {
        pthread_join(threads[i], NULL);
    }

    Nodo_t *aux = test;

    i = 0;

    while (aux != NULL)
    {
        printf("%lf,%lf,%s\n",
               aux->x,
               aux->y,
               resultados[i]);

        aux = aux->siguiente;
        i++;
    }

    FILE *fd = fopen("resultados.csv", "w");

    aux = test;

    i = 0;

    while (aux != NULL)
    {
        fprintf(fd,
                "%lf,%lf,%s\n",
                aux->x,
                aux->y,
                resultados[i]);

        aux = aux->siguiente;
        i++;
    }

    fclose(fd);

    printf("Terminado\n");
}
