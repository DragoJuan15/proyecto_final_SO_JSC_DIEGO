#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include "knn.h"

#define MAX_CLASES 16
#define BUF_LINEA 32768

typedef struct Hilo
{
    double *train_data;
    char (*train_clases)[8];
    int total_train;
    int n_features;
    double *test_data;
    int inicio;
    int fin;
    int k;
    char **resultados;
    char (*clases)[8];
    int n_clases;
} Hilo_t;

double *leer_csv(char *archivo, char (**out_clases)[8], int *out_rows, int *out_features)
{
    FILE *fd = fopen(archivo, "r");
    if (fd == NULL)
    {
        printf("Error: no pude abrir %s\n", archivo);
        return NULL;
    }

    char *linea = (char *)malloc(BUF_LINEA);
    if (fgets(linea, BUF_LINEA, fd) == NULL)
    {
        free(linea);
        fclose(fd);
        return NULL;
    }

    int comas = 0;
    int i;
    for (i = 0; linea[i]; i++)
        if (linea[i] == ',')
            comas++;
    int n_features = comas;
    rewind(fd);

    int capacidad = 4096;
    int n = 0;
    double *data = (double *)malloc(sizeof(double) * capacidad * n_features);
    char (*clases)[8] = (char (*)[8])malloc(sizeof(char[8]) * capacidad);

    while (fgets(linea, BUF_LINEA, fd))
    {
        linea[strcspn(linea, "\r\n")] = 0;
        if (linea[0] == '\0')
            continue;

        if (n >= capacidad)
        {
            capacidad *= 2;
            data = (double *)realloc(data, sizeof(double) * capacidad * n_features);
            clases = (char (*)[8])realloc(clases, sizeof(char[8]) * capacidad);
        }

        char *p = linea;
        char *next;
        int ok = 1;
        int j;
        for (j = 0; j < n_features; j++)
        {
            double v = strtod(p, &next);
            if (next == p)
            {
                ok = 0;
                break;
            }
            data[n * n_features + j] = v;
            p = next;
            if (*p == ',')
                p++;
        }
        if (!ok)
            continue;

        char *e = p;
        while (*e && *e != ',' && *e != '\n' && *e != '\r' && *e != ' ' && *e != '\t')
            e++;
        int len = e - p;
        if (len > 7)
            len = 7;
        memcpy(clases[n], p, len);
        clases[n][len] = '\0';
        n++;
    }

    free(linea);
    fclose(fd);
    *out_clases = clases;
    *out_rows = n;
    *out_features = n_features;
    return data;
}

void *knn_hilo(void *arg)
{
    Hilo_t *d = (Hilo_t *)arg;
    int nf = d->n_features;
    int k = d->k;

    double *dist_topk = (double *)malloc(sizeof(double) * k);
    char (*clase_topk)[8] = (char (*)[8])malloc(sizeof(char[8]) * k);

    int t;
    for (t = d->inicio; t < d->fin; t++)
    {
        double *test_punto = d->test_data + t * nf;
        int llenos = 0;
        double peor = 1e308;
        int i, f, j;

        for (i = 0; i < d->total_train; i++)
        {
            double *tr = d->train_data + i * nf;
            double d2 = 0.0;
            int abort = 0;
            if (llenos >= k)
            {
                for (f = 0; f < nf; f++)
                {
                    double diff = tr[f] - test_punto[f];
                    d2 += diff * diff;
                    if (d2 >= peor)
                    {
                        abort = 1;
                        break;
                    }
                }
                if (abort)
                    continue;
            }
            else
            {
                for (f = 0; f < nf; f++)
                {
                    double diff = tr[f] - test_punto[f];
                    d2 += diff * diff;
                }
            }

            if (llenos < k)
            {
                j = llenos - 1;
                while (j >= 0 && dist_topk[j] > d2)
                {
                    dist_topk[j + 1] = dist_topk[j];
                    memcpy(clase_topk[j + 1], clase_topk[j], 8);
                    j--;
                }
                dist_topk[j + 1] = d2;
                memcpy(clase_topk[j + 1], d->train_clases[i], 8);
                llenos++;
                if (llenos == k)
                    peor = dist_topk[k - 1];
            }
            else
            {
                j = k - 2;
                while (j >= 0 && dist_topk[j] > d2)
                {
                    dist_topk[j + 1] = dist_topk[j];
                    memcpy(clase_topk[j + 1], clase_topk[j], 8);
                    j--;
                }
                dist_topk[j + 1] = d2;
                memcpy(clase_topk[j + 1], d->train_clases[i], 8);
                peor = dist_topk[k - 1];
            }
        }

        int conteos[MAX_CLASES] = {0};
        for (i = 0; i < llenos; i++)
        {
            for (j = 0; j < d->n_clases; j++)
            {
                if (strcmp(clase_topk[i], d->clases[j]) == 0)
                {
                    conteos[j]++;
                    break;
                }
            }
        }
        int max_idx = 0;
        for (j = 1; j < d->n_clases; j++)
        {
            if (conteos[j] > conteos[max_idx])
                max_idx = j;
        }

        char *resultado = (char *)malloc(8);
        memcpy(resultado, d->clases[max_idx], 8);
        d->resultados[t] = resultado;
    }

    free(dist_topk);
    free(clase_topk);
    return NULL;
}

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos)
{
    char (*train_clases)[8] = NULL;
    char (*test_clases)[8] = NULL;
    int total_train = 0, total_test = 0, nf_train = 0, nf_test = 0;

    double *train_data = leer_csv(entrenamiento, &train_clases, &total_train, &nf_train);
    double *test_data = leer_csv(prueba, &test_clases, &total_test, &nf_test);

    if (train_data == NULL || test_data == NULL || total_train == 0 || total_test == 0)
    {
        printf("Datos invalidos\n");
        free(train_data);
        free(train_clases);
        free(test_data);
        free(test_clases);
        return;
    }
    int n_features = nf_train < nf_test ? nf_train : nf_test;

    char clases[MAX_CLASES][8];
    int n_clases = 0;
    int i, j;
    for (i = 0; i < total_train; i++)
    {
        if (train_clases[i][0] == '\0')
            continue;
        int existe = 0;
        for (j = 0; j < n_clases; j++)
            if (strcmp(train_clases[i], clases[j]) == 0)
            {
                existe = 1;
                break;
            }
        if (!existe && n_clases < MAX_CLASES)
        {
            memcpy(clases[n_clases], train_clases[i], 8);
            n_clases++;
        }
    }

    printf("Train=%d Test=%d features=%d k=%d hilos=%d\n", total_train, total_test, n_features, k, hilos);
    printf("Clases (%d): ", n_clases);
    int c;
    for (c = 0; c < n_clases; c++)
        printf("%s%s", clases[c], c == n_clases - 1 ? "\n" : ", ");

    if (hilos < 1)
        hilos = 1;
    if (hilos > total_test)
        hilos = total_test;

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    char **resultados = (char **)calloc(total_test, sizeof(char *));
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * hilos);
    Hilo_t *datos = (Hilo_t *)malloc(sizeof(Hilo_t) * hilos);

    int bloque = total_test / hilos;
    int inicio = 0;

    for (i = 0; i < hilos; i++)
    {
        datos[i].train_data = train_data;
        datos[i].train_clases = train_clases;
        datos[i].total_train = total_train;
        datos[i].n_features = n_features;
        datos[i].test_data = test_data;
        datos[i].inicio = inicio;
        datos[i].fin = (i == hilos - 1) ? total_test : inicio + bloque;
        datos[i].k = k;
        datos[i].resultados = resultados;
        datos[i].clases = clases;
        datos[i].n_clases = n_clases;
        pthread_create(&threads[i], NULL, knn_hilo, (void *)&datos[i]);
        inicio = datos[i].fin;
    }
    for (i = 0; i < hilos; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&t1, NULL);
    double seg = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("Tiempo: %.3f s\n", seg);

    int correctos = 0, evaluados = 0;
    int matriz[MAX_CLASES][MAX_CLASES] = {{0}};
    for (i = 0; i < total_test; i++)
    {
        if (test_clases[i][0] == '\0')
            continue;
        int idx_real = -1, idx_pred = -1;
        for (j = 0; j < n_clases; j++)
        {
            if (strcmp(test_clases[i], clases[j]) == 0)
                idx_real = j;
            if (resultados[i] && strcmp(resultados[i], clases[j]) == 0)
                idx_pred = j;
        }
        if (idx_real < 0)
            continue;
        evaluados++;
        if (idx_pred == idx_real)
            correctos++;
        if (idx_pred >= 0)
            matriz[idx_real][idx_pred]++;
    }
    double accuracy = evaluados > 0 ? (double)correctos / evaluados : 0.0;

    printf("\n--- Matriz de confusion ---\n         ");
    for (c = 0; c < n_clases; c++)
        printf("%8s", clases[c]);
    printf("\n");
    int r;
    for (r = 0; r < n_clases; r++)
    {
        printf("%8s ", clases[r]);
        for (c = 0; c < n_clases; c++)
            printf("%8d", matriz[r][c]);
        printf("\n");
    }
    printf("Accuracy: %.4f  (%d / %d)\n", accuracy, correctos, evaluados);

    FILE *fm = fopen("metricas.csv", "a");
    if (fm != NULL)
    {
        fseek(fm, 0, SEEK_END);
        if (ftell(fm) == 0)
            fprintf(fm, "hilos,k,total,tiempo_s,correctos,accuracy\n");
        fprintf(fm, "%d,%d,%d,%.3f,%d,%.4f\n", hilos, k, evaluados, seg, correctos, accuracy);
        fclose(fm);
    }

    char nombre_archivo[64];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "resultados_h%d_k%d.csv", hilos, k);
    FILE *fd = fopen(nombre_archivo, "w");
    if (fd != NULL)
    {
        for (i = 0; i < total_test; i++)
            fprintf(fd, "%s,%s\n", test_clases[i][0] ? test_clases[i] : "?", resultados[i] ? resultados[i] : "?");
        fclose(fd);
    }

    for (i = 0; i < total_test; i++)
        free(resultados[i]);
    free(resultados);
    free(threads);
    free(datos);
    free(train_data);
    free(train_clases);
    free(test_data);
    free(test_clases);

    printf("Terminado. Resultados en %s\n", nombre_archivo);
}
