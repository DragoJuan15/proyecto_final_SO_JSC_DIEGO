#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include "knn.h"

typedef struct Punto
{
    double x;
    double y;
    char clase[8];
} Punto_t;

typedef struct Distancia
{
    double dist2;
    char clase[8];
} Distancia_t;

typedef struct Hilo
{
    Punto_t *train;
    int total_train;
    Punto_t *test;
    int inicio;
    int fin;
    int k;
    char **resultados;
} Hilo_t;

static Punto_t *leer_csv_array(const char *archivo, int *total)
{
    FILE *fd = fopen(archivo, "r");
    if (fd == NULL)
    {
        printf("Error: no pude abrir %s\n", archivo);
        *total = 0;
        return NULL;
    }

    int capacidad = 1024;
    int n = 0;
    Punto_t *arr = (Punto_t *)malloc(sizeof(Punto_t) * capacidad);
    if (arr == NULL)
    {
        fclose(fd);
        *total = 0;
        return NULL;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), fd))
    {
        linea[strcspn(linea, "\r\n")] = 0;
        if (linea[0] == '\0')
            continue;

        double x, y;
        char clase[8] = "";
        int leidos = sscanf(linea, "%lf,%lf,%7s", &x, &y, clase);
        if (leidos < 2)
            continue;

        if (n >= capacidad)
        {
            capacidad *= 2;
            Punto_t *tmp = (Punto_t *)realloc(arr, sizeof(Punto_t) * capacidad);
            if (tmp == NULL)
            {
                free(arr);
                fclose(fd);
                *total = 0;
                return NULL;
            }
            arr = tmp;
        }

        arr[n].x = x;
        arr[n].y = y;
        strncpy(arr[n].clase, clase, sizeof(arr[n].clase) - 1);
        arr[n].clase[sizeof(arr[n].clase) - 1] = '\0';
        n++;
    }

    fclose(fd);
    *total = n;
    return arr;
}

static inline double dist2_euclidiana(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return dx * dx + dy * dy;
}

static int cmp_distancia(const void *a, const void *b)
{
    double da = ((const Distancia_t *)a)->dist2;
    double db = ((const Distancia_t *)b)->dist2;
    if (da < db)
        return -1;
    if (da > db)
        return 1;
    return 0;
}

static char *clasificar(Punto_t *train, int total_train,
                        Punto_t *test_punto, int k,
                        Distancia_t *buffer)
{
    int i;

    for (i = 0; i < total_train; i++)
    {
        buffer[i].dist2 = dist2_euclidiana(
            test_punto->x, test_punto->y,
            train[i].x, train[i].y);

        strncpy(buffer[i].clase, train[i].clase, sizeof(buffer[i].clase) - 1);
        buffer[i].clase[sizeof(buffer[i].clase) - 1] = '\0';
    }

    qsort(buffer, total_train, sizeof(Distancia_t), cmp_distancia);

    int contadorA = 0;
    int contadorB = 0;
    int limite = (k < total_train) ? k : total_train;

    for (i = 0; i < limite; i++)
    {
        if (strcmp(buffer[i].clase, "A") == 0)
            contadorA++;
        else if (strcmp(buffer[i].clase, "B") == 0)
            contadorB++;
    }

    char *resultado = (char *)malloc(8);
    strcpy(resultado, (contadorA > contadorB) ? "A" : "B");
    return resultado;
}

static void *knn_hilo(void *arg)
{
    Hilo_t *datos = (Hilo_t *)arg;

    Distancia_t *buffer = (Distancia_t *)malloc(sizeof(Distancia_t) * datos->total_train);
    if (buffer == NULL)
        return NULL;

    int i;
    for (i = datos->inicio; i < datos->fin; i++)
    {
        datos->resultados[i] = clasificar(
            datos->train, datos->total_train,
            &datos->test[i], datos->k,
            buffer);
    }

    free(buffer);
    return NULL;
}

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos)
{
    int total_train = 0;
    int total_test = 0;

    Punto_t *train = leer_csv_array(entrenamiento, &total_train);
    Punto_t *test = leer_csv_array(prueba, &total_test);

    if (train == NULL || test == NULL || total_train == 0 || total_test == 0)
    {
        printf("Datos vacios o archivos invalidos\n");
        free(train);
        free(test);
        return;
    }

    if (hilos < 1)
        hilos = 1;
    if (hilos > total_test)
        hilos = total_test;

    printf("Train=%d  Test=%d  k=%d  hilos=%d\n",
           total_train, total_test, k, hilos);

    double mem_train = (double)total_train * sizeof(Punto_t) / 1024.0;
    double mem_test = (double)total_test * sizeof(Punto_t) / 1024.0;
    double mem_buf = (double)hilos * total_train * sizeof(Distancia_t) / 1024.0;
    printf("Memoria: train=%.1f KB  test=%.1f KB  buffers=%.1f KB\n",
           mem_train, mem_test, mem_buf);

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    char **resultados = (char **)calloc(total_test, sizeof(char *));
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * hilos);
    Hilo_t *datos = (Hilo_t *)malloc(sizeof(Hilo_t) * hilos);

    int bloque = total_test / hilos;
    int inicio = 0;
    int i;

    for (i = 0; i < hilos; i++)
    {
        datos[i].train = train;
        datos[i].total_train = total_train;
        datos[i].test = test;
        datos[i].inicio = inicio;
        datos[i].fin = (i == hilos - 1) ? total_test : inicio + bloque;
        datos[i].k = k;
        datos[i].resultados = resultados;

        pthread_create(&threads[i], NULL, knn_hilo, (void *)&datos[i]);
        inicio = datos[i].fin;
    }

    for (i = 0; i < hilos; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&t1, NULL);
    double seg = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("Tiempo real (wall clock): %.3f s\n", seg);

    FILE *fd = fopen("resultados.csv", "w");
    if (fd != NULL)
    {
        for (i = 0; i < total_test; i++)
        {
            fprintf(fd, "%lf,%lf,%s\n",
                    test[i].x, test[i].y,
                    resultados[i] ? resultados[i] : "?");
        }
        fclose(fd);
    }

    for (i = 0; i < total_test; i++)
        free(resultados[i]);
    free(resultados);
    free(threads);
    free(datos);
    free(train);
    free(test);

    printf("Terminado. Resultados en resultados.csv\n");
}