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

int contar_filas(char *archivo)
{
    FILE *fd = fopen(archivo, "r");
    if (fd == NULL) return 0;
    int n = 0;
    char *linea = (char *)malloc(BUF_LINEA);
    while (fgets(linea, BUF_LINEA, fd))
    {
        if (linea[0] != '\0' && linea[0] != '\n' && linea[0] != '\r') n++;
    }
    free(linea);
    fclose(fd);
    return n;
}

int contar_columnas(char *archivo)
{
    FILE *fd = fopen(archivo, "r");
    if (fd == NULL) return 0;
    int comas = 0;
    char *linea = (char *)malloc(BUF_LINEA);
    if (fgets(linea, BUF_LINEA, fd))
    {
        int i;
        for (i = 0; linea[i]; i++) if (linea[i] == ',') comas++;
    }
    free(linea);
    fclose(fd);
    return comas;
}

void leer_datos(char *archivo, double *data, char (*clases)[8], int n_features)
{
    FILE *fd = fopen(archivo, "r");
    if (fd == NULL) return;
    char *linea = (char *)malloc(BUF_LINEA);
    int n = 0;

    while (fgets(linea, BUF_LINEA, fd))
    {
        if (linea[0] == '\0' || linea[0] == '\n' || linea[0] == '\r') continue;

        int len = strlen(linea);
        while (len > 0 && (linea[len - 1] == '\n' || linea[len - 1] == '\r'))
        {
            linea[len - 1] = '\0';
            len--;
        }

        char *token = strtok(linea, ",");
        int j = 0;
        while (token != NULL && j < n_features)
        {
            data[n * n_features + j] = atof(token);
            token = strtok(NULL, ",");
            j++;
        }
        if (token != NULL && strlen(token) < 8) strcpy(clases[n], token);
        else clases[n][0] = '\0';
        n++;
    }

    free(linea);
    fclose(fd);
}

int recolectar_clases(char (*train_clases)[8], int total_train, char (*clases)[8])
{
    int n = 0;
    int i, j;
    for (i = 0; i < total_train; i++)
    {
        if (train_clases[i][0] == '\0') continue;
        int existe = 0;
        for (j = 0; j < n; j++)
        {
            if (strcmp(train_clases[i], clases[j]) == 0) { existe = 1; break; }
        }
        if (!existe && n < MAX_CLASES)
        {
            strcpy(clases[n], train_clases[i]);
            n++;
        }
    }
    return n;
}

double calcular_distancia(double *a, double *b, int n_features)
{
    double sum = 0.0;
    int i;
    for (i = 0; i < n_features; i++)
    {
        double d = a[i] - b[i];
        sum += d * d;
    }
    return sum;
}

char *clasificar(double *train_data, char (*train_clases)[8], int total_train, double *test_punto, int k, int n_features, char (*clases)[8], int n_clases)
{
    double *dist_topk = (double *)malloc(sizeof(double) * k);
    char (*clase_topk)[8] = (char (*)[8])malloc(sizeof(char[8]) * k);
    int llenos = 0;
    int i, j;

    for (i = 0; i < total_train; i++)
    {
        double d2 = calcular_distancia(train_data + i * n_features, test_punto, n_features);

        if (llenos < k)
        {
            j = llenos - 1;
            while (j >= 0 && dist_topk[j] > d2)
            {
                dist_topk[j + 1] = dist_topk[j];
                strcpy(clase_topk[j + 1], clase_topk[j]);
                j--;
            }
            dist_topk[j + 1] = d2;
            strcpy(clase_topk[j + 1], train_clases[i]);
            llenos++;
        }
        else if (d2 < dist_topk[k - 1])
        {
            j = k - 2;
            while (j >= 0 && dist_topk[j] > d2)
            {
                dist_topk[j + 1] = dist_topk[j];
                strcpy(clase_topk[j + 1], clase_topk[j]);
                j--;
            }
            dist_topk[j + 1] = d2;
            strcpy(clase_topk[j + 1], train_clases[i]);
        }
    }

    int conteos[MAX_CLASES];
    for (i = 0; i < MAX_CLASES; i++) conteos[i] = 0;
    for (i = 0; i < llenos; i++)
    {
        for (j = 0; j < n_clases; j++)
        {
            if (strcmp(clase_topk[i], clases[j]) == 0) { conteos[j]++; break; }
        }
    }
    int max_idx = 0;
    for (j = 1; j < n_clases; j++)
    {
        if (conteos[j] > conteos[max_idx]) max_idx = j;
    }

    char *resultado = (char *)malloc(8);
    strcpy(resultado, clases[max_idx]);

    free(dist_topk);
    free(clase_topk);
    return resultado;
}

void *knn_hilo(void *arg)
{
    Hilo_t *d = (Hilo_t *)arg;
    int i;
    for (i = d->inicio; i < d->fin; i++)
    {
        double *test_punto = d->test_data + i * d->n_features;
        d->resultados[i] = clasificar(d->train_data, d->train_clases, d->total_train, test_punto, d->k, d->n_features, d->clases, d->n_clases);
    }
    return NULL;
}

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos)
{
    int total_train = contar_filas(entrenamiento);
    int total_test = contar_filas(prueba);
    int n_features_train = contar_columnas(entrenamiento);
    int n_features_test = contar_columnas(prueba);

    if (total_train == 0 || total_test == 0 || n_features_train == 0)
    {
        printf("Archivos invalidos\n");
        return;
    }

    int n_features = n_features_train < n_features_test ? n_features_train : n_features_test;

    double *train_data = (double *)malloc(sizeof(double) * total_train * n_features);
    char (*train_clases)[8] = (char (*)[8])malloc(sizeof(char[8]) * total_train);
    double *test_data = (double *)malloc(sizeof(double) * total_test * n_features);
    char (*test_clases)[8] = (char (*)[8])malloc(sizeof(char[8]) * total_test);

    leer_datos(entrenamiento, train_data, train_clases, n_features);
    leer_datos(prueba, test_data, test_clases, n_features);

    char clases[MAX_CLASES][8];
    int n_clases = recolectar_clases(train_clases, total_train, clases);

    printf("Train=%d Test=%d features=%d k=%d hilos=%d\n", total_train, total_test, n_features, k, hilos);
    printf("Clases (%d): ", n_clases);
    int c;
    for (c = 0; c < n_clases; c++) printf("%s%s", clases[c], c == n_clases - 1 ? "\n" : ", ");

    if (hilos < 1) hilos = 1;
    if (hilos > total_test) hilos = total_test;

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    char **resultados = (char **)malloc(sizeof(char *) * total_test);
    int idx;
    for (idx = 0; idx < total_test; idx++) resultados[idx] = NULL;

    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * hilos);
    Hilo_t *datos = (Hilo_t *)malloc(sizeof(Hilo_t) * hilos);

    int bloque = total_test / hilos;
    int inicio = 0;
    int i;

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
    for (i = 0; i < hilos; i++) pthread_join(threads[i], NULL);

    gettimeofday(&t1, NULL);
    double seg = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("Tiempo: %.3f s\n", seg);

    int correctos = 0, evaluados = 0;
    int matriz[MAX_CLASES][MAX_CLASES];
    int r;
    for (r = 0; r < MAX_CLASES; r++) for (c = 0; c < MAX_CLASES; c++) matriz[r][c] = 0;

    int j;
    for (i = 0; i < total_test; i++)
    {
        if (test_clases[i][0] == '\0') continue;
        int idx_real = -1, idx_pred = -1;
        for (j = 0; j < n_clases; j++)
        {
            if (strcmp(test_clases[i], clases[j]) == 0) idx_real = j;
            if (resultados[i] && strcmp(resultados[i], clases[j]) == 0) idx_pred = j;
        }
        if (idx_real < 0) continue;
        evaluados++;
        if (idx_pred == idx_real) correctos++;
        if (idx_pred >= 0) matriz[idx_real][idx_pred]++;
    }

    double accuracy = evaluados > 0 ? (double)correctos / evaluados : 0.0;

    printf("\n--- Matriz de confusion ---\n         ");
    for (c = 0; c < n_clases; c++) printf("%8s", clases[c]);
    printf("\n");
    for (r = 0; r < n_clases; r++)
    {
        printf("%8s ", clases[r]);
        for (c = 0; c < n_clases; c++) printf("%8d", matriz[r][c]);
        printf("\n");
    }
    printf("Accuracy: %.4f  (%d / %d)\n", accuracy, correctos, evaluados);

    FILE *fm = fopen("metricas.csv", "a");
    if (fm != NULL)
    {
        fseek(fm, 0, SEEK_END);
        if (ftell(fm) == 0) fprintf(fm, "hilos,k,total,tiempo_s,correctos,accuracy\n");
        fprintf(fm, "%d,%d,%d,%.3f,%d,%.4f\n", hilos, k, evaluados, seg, correctos, accuracy);
        fclose(fm);
    }

    char nombre_archivo[64];
    sprintf(nombre_archivo, "resultados_h%d_k%d.csv", hilos, k);
    FILE *fd = fopen(nombre_archivo, "w");
    if (fd != NULL)
    {
        for (i = 0; i < total_test; i++)
            fprintf(fd, "%s,%s\n", test_clases[i][0] ? test_clases[i] : "?", resultados[i] ? resultados[i] : "?");
        fclose(fd);
    }

    for (i = 0; i < total_test; i++) free(resultados[i]);
    free(resultados); free(threads); free(datos);
    free(train_data); free(train_clases); free(test_data); free(test_clases);

    printf("Terminado. Resultados en %s\n", nombre_archivo);
}