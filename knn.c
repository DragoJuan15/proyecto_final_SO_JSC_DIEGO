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

typedef struct Vecino
{
    double dist2;
    char clase[8];
} Vecino_t;

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

Punto_t *leer_csv_array(const char *archivo, int *total)
{
    FILE *fd = fopen(archivo, "r");
    if (fd == NULL) { printf("Error: no pude abrir %s\n", archivo); *total = 0; return NULL; }

    int capacidad = 1024;
    int n = 0;
    Punto_t *arr = (Punto_t *)malloc(sizeof(Punto_t) * capacidad);
    if (arr == NULL) { fclose(fd); *total = 0; return NULL; }

    char linea[256];
    while (fgets(linea, sizeof(linea), fd))
    {
        linea[strcspn(linea, "\r\n")] = 0;
        if (linea[0] == '\0') continue;

        double x, y;
        char clase[8] = "";
        int leidos = sscanf(linea, "%lf,%lf,%7s", &x, &y, clase);
        if (leidos < 2) continue;

        if (n >= capacidad)
        {
            capacidad *= 2;
            Punto_t *tmp = (Punto_t *)realloc(arr, sizeof(Punto_t) * capacidad);
            if (tmp == NULL) { free(arr); fclose(fd); *total = 0; return NULL; }
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

char *clasificar(Punto_t *train, int total_train, Punto_t *test_punto, int k, Vecino_t *topk)
{
    int i, j;
    int llenos = 0;
    double peor = 1e308;

    for (i = 0; i < total_train; i++)
    {
        double dx = train[i].x - test_punto->x;
        double dy = train[i].y - test_punto->y;
        double d2 = dx * dx + dy * dy;

        if (llenos < k)
        {
            j = llenos - 1;
            while (j >= 0 && topk[j].dist2 > d2)
            {
                topk[j + 1] = topk[j];
                j--;
            }
            topk[j + 1].dist2 = d2;
            strncpy(topk[j + 1].clase, train[i].clase, sizeof(topk[j + 1].clase) - 1);
            topk[j + 1].clase[sizeof(topk[j + 1].clase) - 1] = '\0';
            llenos++;
            if (llenos == k) peor = topk[k - 1].dist2;
        }
        else if (d2 < peor)
        {
            j = k - 2;
            while (j >= 0 && topk[j].dist2 > d2)
            {
                topk[j + 1] = topk[j];
                j--;
            }
            topk[j + 1].dist2 = d2;
            strncpy(topk[j + 1].clase, train[i].clase, sizeof(topk[j + 1].clase) - 1);
            topk[j + 1].clase[sizeof(topk[j + 1].clase) - 1] = '\0';
            peor = topk[k - 1].dist2;
        }
    }

    int contadorA = 0, contadorB = 0;
    for (i = 0; i < llenos; i++)
    {
        if (strcmp(topk[i].clase, "A") == 0) contadorA++;
        else if (strcmp(topk[i].clase, "B") == 0) contadorB++;
    }

    char *resultado = (char *)malloc(8);
    strcpy(resultado, (contadorA > contadorB) ? "A" : "B");
    return resultado;
}

void *knn_hilo(void *arg)
{
    Hilo_t *datos = (Hilo_t *)arg;
    Vecino_t *topk = (Vecino_t *)malloc(sizeof(Vecino_t) * datos->k);
    if (topk == NULL) return NULL;

    int i;
    for (i = datos->inicio; i < datos->fin; i++)
        datos->resultados[i] = clasificar(datos->train, datos->total_train, &datos->test[i], datos->k, topk);

    free(topk);
    return NULL;
}

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos)
{
    int total_train = 0, total_test = 0;
    Punto_t *train = leer_csv_array(entrenamiento, &total_train);
    Punto_t *test  = leer_csv_array(prueba, &total_test);

    if (train == NULL || test == NULL || total_train == 0 || total_test == 0)
    {
        printf("Datos vacios o archivos invalidos\n");
        free(train); free(test);
        return;
    }

    if (hilos < 1) hilos = 1;
    if (hilos > total_test) hilos = total_test;

    printf("Train=%d Test=%d k=%d hilos=%d\n", total_train, total_test, k, hilos);

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

    for (i = 0; i < hilos; i++) pthread_join(threads[i], NULL);

    gettimeofday(&t1, NULL);
    double seg = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("Tiempo: %.3f s\n", seg);

    int tiene_etiquetas = 0;
    for (i = 0; i < total_test; i++)
    {
        if (test[i].clase[0] != '\0') { tiene_etiquetas = 1; break; }
    }

    int tp = 0, tn = 0, fp = 0, fn = 0, sin_clase = 0;
    if (tiene_etiquetas)
    {
        for (i = 0; i < total_test; i++)
        {
            const char *real = test[i].clase;
            const char *pred = resultados[i] ? resultados[i] : "?";
            if (real[0] == '\0') { sin_clase++; continue; }
            if (strcmp(real, "A") == 0 && strcmp(pred, "A") == 0) tp++;
            else if (strcmp(real, "B") == 0 && strcmp(pred, "B") == 0) tn++;
            else if (strcmp(real, "B") == 0 && strcmp(pred, "A") == 0) fp++;
            else if (strcmp(real, "A") == 0 && strcmp(pred, "B") == 0) fn++;
        }

        int evaluados = tp + tn + fp + fn;
        double accuracy  = evaluados > 0 ? (double)(tp + tn) / evaluados : 0.0;
        double precision = (tp + fp) > 0 ? (double)tp / (tp + fp) : 0.0;
        double recall    = (tp + fn) > 0 ? (double)tp / (tp + fn) : 0.0;
        double f1        = (precision + recall) > 0 ? 2.0 * precision * recall / (precision + recall) : 0.0;

        printf("\n--- Metricas (A = positivo) ---\n");
        printf("Matriz de confusion:\n");
        printf("                Pred A    Pred B\n");
        printf("  Real A   TP=%6d  FN=%6d\n", tp, fn);
        printf("  Real B   FP=%6d  TN=%6d\n", fp, tn);
        printf("Accuracy : %.4f  (%d / %d)\n", accuracy, tp + tn, evaluados);
        printf("Precision: %.4f\n", precision);
        printf("Recall   : %.4f\n", recall);
        printf("F1-score : %.4f\n", f1);
        if (sin_clase > 0) printf("Sin clase real: %d (ignorados)\n", sin_clase);

        FILE *fm = fopen("metricas.csv", "a");
        if (fm != NULL)
        {
            fseek(fm, 0, SEEK_END);
            if (ftell(fm) == 0)
                fprintf(fm, "hilos,k,total_test,tiempo_s,tp,tn,fp,fn,accuracy,precision,recall,f1\n");
            fprintf(fm, "%d,%d,%d,%.3f,%d,%d,%d,%d,%.4f,%.4f,%.4f,%.4f\n", hilos, k, evaluados, seg, tp, tn, fp, fn, accuracy, precision, recall, f1);
            fclose(fm);
            printf("Metricas anexadas a metricas.csv\n");
        }
    }
    else
    {
        printf("Aviso: el set de prueba no trae clase real, no se calcularon metricas.\n");

        FILE *fm = fopen("metricas.csv", "a");
        if (fm != NULL)
        {
            fseek(fm, 0, SEEK_END);
            if (ftell(fm) == 0)
                fprintf(fm, "hilos,k,total_test,tiempo_s,tp,tn,fp,fn,accuracy,precision,recall,f1\n");
            fprintf(fm, "%d,%d,%d,%.3f,,,,,,,,\n", hilos, k, total_test, seg);
            fclose(fm);
        }
    }

    char nombre_archivo[64];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "resultados_h%d_k%d.csv", hilos, k);

    FILE *fd = fopen(nombre_archivo, "w");
    if (fd != NULL)
    {
        if (tiene_etiquetas)
        {
            for (i = 0; i < total_test; i++)
                fprintf(fd, "%lf,%lf,%s,%s\n", test[i].x, test[i].y, test[i].clase[0] ? test[i].clase : "?", resultados[i] ? resultados[i] : "?");
        }
        else
        {
            for (i = 0; i < total_test; i++)
                fprintf(fd, "%lf,%lf,%s\n", test[i].x, test[i].y, resultados[i] ? resultados[i] : "?");
        }
        fclose(fd);
    }

    for (i = 0; i < total_test; i++) free(resultados[i]);
    free(resultados); free(threads); free(datos); free(train); free(test);

    printf("Terminado. Resultados en %s\n", nombre_archivo);
}