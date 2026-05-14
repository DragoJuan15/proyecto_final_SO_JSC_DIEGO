#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "knn.h"

void knnJSC(char training_csv[], char test_csv[], int k, int num_hilos)
{
    Data_t train[2048];
    Data_t test[2048];

    int train_n = 0;
    int test_n = 0;

    cargar_csv(
        training_csv,
        train,
        &train_n
    );

    cargar_csv(
        test_csv,
        test,
        &test_n
    );

    pthread_t threads[128];
    HiloKNN_t hilos[128];

    int bloque = test_n / num_hilos;

    for (int i = 0; i < num_hilos; i++)
    {
        hilos[i].train = train;
        hilos[i].test = test;

        hilos[i].train_n = train_n;

        hilos[i].inicio =
            i * bloque;

        if (i == num_hilos - 1)
        {
            hilos[i].fin = test_n;
        }
        else
        {
            hilos[i].fin =
                (i + 1) * bloque;
        }

        hilos[i].k = k;

        pthread_create(
            &threads[i],
            NULL,
            knnJSC_thread,
            &hilos[i]
        );
    }

    for (int i = 0; i < num_hilos; i++)
    {
        pthread_join(
            threads[i],
            NULL
        );
    }

    int correctos = 0;

    for (int i = 0; i < test_n; i++)
    {
        printf(
            "Dato %d -> Real:%d Pred:%d\n",
            i + 1,
            test[i].clase_real,
            test[i].clase_predicha
        );

        if (
            test[i].clase_real ==
            test[i].clase_predicha
        )
        {
            correctos++;
        }
    }

    float accuracy =
        ((float)correctos / test_n)
        * 100;

    printf(
        "\nAccuracy %.2f%%\n",
        accuracy
    );

    guardar_resultados(
        "resultados_knn.csv",
        test,
        test_n,
        accuracy
    );
}

void cargar_csv(char nombre[], Data_t datos[], int *n)
{
    FILE *fd =
        fopen(nombre, "r");

    if (fd == NULL)
    {
        printf("Error archivo\n");
        return;
    }

    int i = 0;

    while (
        fscanf(
            fd,
            "%f,%f,%d",
            &datos[i].x,
            &datos[i].y,
            &datos[i].clase_real
        ) == 3
    )
    {
        datos[i].clase_predicha = -1;
        i++;
    }

    fclose(fd);

    i--;

    *n = i;
}

float distancia_euclideana(Data_t a, Data_t b)
{
    return sqrt(
        pow(a.x - b.x, 2)
        +
        pow(a.y - b.y, 2)
    );
}

void ordenar_vecinos(Vecino_t vecinos[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (
                vecinos[j].distancia
                <
                vecinos[i].distancia
            )
            {
                Vecino_t temp =
                    vecinos[i];

                vecinos[i] =
                    vecinos[j];

                vecinos[j] =
                    temp;
            }
        }
    }
}

int knn(Data_t train[], int train_n, Data_t test, int k)
{
    Vecino_t vecinos[2048];

    for (int i = 0; i < train_n; i++)
    {
        vecinos[i].distancia =
            distancia_euclideana(
                train[i],
                test
            );

        vecinos[i].clase =
            train[i].clase_real;
    }

    ordenar_vecinos(
        vecinos,
        train_n
    );

    int clases[100] = {0};

    for (int i = 0; i < k; i++)
    {
        clases[
            vecinos[i].clase
        ]++;
    }

    int mayor = 0;
    int clase = 0;

    for (int i = 0; i < 100; i++)
    {
        if (clases[i] > mayor)
        {
            mayor = clases[i];
            clase = i;
        }
    }

    return clase;
}

void *knnJSC_thread(void *hilo_void)
{
    HiloKNN_t *hilo =
        (HiloKNN_t *)hilo_void;

    for (
        int i = hilo->inicio;
        i < hilo->fin;
        i++
    )
    {
        hilo->test[i]
            .clase_predicha =
                knn(
                    hilo->train,
                    hilo->train_n,
                    hilo->test[i],
                    hilo->k
                );
    }

    return NULL;
}

void guardar_resultados(char nombre[], Data_t test[], int n, float accuracy)
{
    FILE *fd =
        fopen(nombre, "w");

    if (fd == NULL)
    {
        return;
    }

    fprintf(
        fd,
        "X,Y,REAL,PRED\n"
    );

    for (int i = 0; i < n; i++)
    {
        fprintf(
            fd,
            "%f,%f,%d,%d\n",
            test[i].x,
            test[i].y,
            test[i].clase_real,
            test[i].clase_predicha
        );
    }

    fprintf(
        fd,
        "\nAccuracy %.2f%%\n",
        accuracy
    );

    fclose(fd);
}
