#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#define MAX_DATOS 1024
#define MAX_LINEA 256

typedef struct Dato
{
    double x;
    double y;
    char clase[50];
} Dato_t;

Dato_t training[MAX_DATOS];
Dato_t test[MAX_DATOS];

int total_training = 0;
int total_test = 0;

int k_global;

int inicio_hilo[1024];
int fin_hilo[1024];

char resultados[1024][50];

double distancia_euclideana(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void cargar_training()
{
    FILE *fd = fopen("trn_set.csv", "r");

    if (fd == NULL)
    {
        printf("No existe trn_set.csv\n");
        return;
    }

    char linea[MAX_LINEA];

    while (fgets(linea, sizeof(linea), fd))
    {
        if (strlen(linea) <= 1)
        {
            continue;
        }

        sscanf(linea, "%lf,%lf,%49s", &training[total_training].x, &training[total_training].y, training[total_training].clase);

        total_training++;
    }

    total_training--;

    fclose(fd);
}

void cargar_test()
{
    FILE *fd = fopen("tst_set.csv", "r");

    if (fd == NULL)
    {
        printf("No existe tst_set.csv\n");
        return;
    }

    char linea[MAX_LINEA];

    while (fgets(linea, sizeof(linea), fd))
    {
        if (strlen(linea) <= 1)
        {
            continue;
        }

        sscanf(linea, "%lf,%lf,%49s", &test[total_test].x, &test[total_test].y, test[total_test].clase);

        total_test++;
    }

    total_test--;

    fclose(fd);
}

void ordenar(double distancias[], char clases[][50], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (distancias[j] > distancias[j + 1])
            {
                double temp_dist = distancias[j];
                distancias[j] = distancias[j + 1];
                distancias[j + 1] = temp_dist;

                char temp_clase[50];

                strcpy(temp_clase, clases[j]);
                strcpy(clases[j], clases[j + 1]);
                strcpy(clases[j + 1], temp_clase);
            }
        }
    }
}

char *predecir(double x, double y, int k)
{
    static char clase_final[50];

    double distancias[MAX_DATOS];
    char clases[MAX_DATOS][50];

    for (int i = 0; i < total_training; i++)
    {
        distancias[i] = distancia_euclideana(x, y, training[i].x, training[i].y);

        strcpy(clases[i], training[i].clase);
    }

    ordenar(distancias, clases, total_training);

    int contadorA = 0;
    int contadorB = 0;
    int contadorC = 0;
    int contadorD = 0;

    for (int i = 0; i < k; i++)
    {
        if (strcmp(clases[i], "A") == 0)
        {
            contadorA++;
        }
        else if (strcmp(clases[i], "B") == 0)
        {
            contadorB++;
        }
        else if (strcmp(clases[i], "C") == 0)
        {
            contadorC++;
        }
        else if (strcmp(clases[i], "D") == 0)
        {
            contadorD++;
        }
    }

    int mayor = contadorA;

    strcpy(clase_final, "A");

    if (contadorB > mayor)
    {
        mayor = contadorB;
        strcpy(clase_final, "B");
    }

    if (contadorC > mayor)
    {
        mayor = contadorC;
        strcpy(clase_final, "C");
    }

    if (contadorD > mayor)
    {
        mayor = contadorD;
        strcpy(clase_final, "D");
    }

    return clase_final;
}

void *knn_thread(void *tokens)
{
    int id = *((int *)tokens);

    for (int i = inicio_hilo[id]; i < fin_hilo[id]; i++)
    {
        strcpy(resultados[i], predecir(test[i].x, test[i].y, k_global));

        printf("Test %d -> %s\n", i, resultados[i]);
    }

    return NULL;
}

void guardar_resultados(int k, int num_hilos)
{
    char nombre[100];

    sprintf(nombre, "resultado_k%d_h%d.csv", k, num_hilos);

    FILE *fd = fopen(nombre, "w");

    if (fd == NULL)
    {
        printf("Error creando CSV\n");
        return;
    }

    fprintf(fd, "X,Y,Clase_Real,Prediccion\n");

    for (int i = 0; i < total_test; i++)
    {
        fprintf(fd, "%lf,%lf,%s,%s\n", test[i].x, test[i].y, test[i].clase, resultados[i]);
    }

    fclose(fd);

    printf("\nResultados guardados en %s\n", nombre);
}

void knnJSC(int k, int num_hilos)
{
    total_training = 0;
    total_test = 0;

    k_global = k;

    cargar_training();
    cargar_test();

    pthread_t hilos[1024];
    int tokens[1024];

    int bloque = total_test / num_hilos;
    int inicio = 0;

    for (int i = 0; i < num_hilos; i++)
    {
        inicio_hilo[i] = inicio;

        fin_hilo[i] = inicio + bloque;

        if (i == num_hilos - 1)
        {
            fin_hilo[i] = total_test;
        }

        inicio = fin_hilo[i];
    }

    for (int i = 0; i < num_hilos; i++)
    {
        tokens[i] = i;

        pthread_create(&hilos[i], NULL, knn_thread, &tokens[i]);
    }

    for (int i = 0; i < num_hilos; i++)
    {
        pthread_join(hilos[i], NULL);
    }

    guardar_resultados(k, num_hilos);
}

