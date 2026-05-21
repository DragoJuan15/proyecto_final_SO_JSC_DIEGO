#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include "knn.h"

int abrir_lector(Lector_t *l, char *archivo) {
    l->fd = open(archivo, O_RDONLY);
    if (l->fd < 0) return 0;
    l->pos = 0;
    l->len = 0;
    return 1;
}

int leer_linea(Lector_t *l, char *linea, int max) {
    int n = 0;
    while (n < max - 1) {
        if (l->pos >= l->len) {
            l->len = read(l->fd, l->buffer, BUF_READ);
            l->pos = 0;
            if (l->len <= 0) { if (n == 0) return 0; break; }
        }
        char c = l->buffer[l->pos++];
        linea[n++] = c;
        if (c == '\n') break;
    }
    linea[n] = '\0';
    return 1;
}

void cerrar_lector(Lector_t *l) {
    close(l->fd);
}

int contar_filas(char *archivo) {
    Lector_t l;
    if (!abrir_lector(&l, archivo)) return 0;
    int n = 0;
    char *linea = (char *)malloc(BUF_LINEA);
    while (leer_linea(&l, linea, BUF_LINEA))
        if (linea[0] != '\0' && linea[0] != '\n' && linea[0] != '\r') n++;
    free(linea); cerrar_lector(&l);
    return n;
}

int contar_columnas(char *archivo) {
    Lector_t l;
    if (!abrir_lector(&l, archivo)) return 0;
    int comas = 0, i;
    char *linea = (char *)malloc(BUF_LINEA);
    if (leer_linea(&l, linea, BUF_LINEA))
        for (i = 0; linea[i]; i++) if (linea[i] == ',') comas++;
    free(linea); cerrar_lector(&l);
    return comas;
}

void leer_datos(char *archivo, double *data, char (*clases)[8], int n_features) {
    Lector_t l;
    if (!abrir_lector(&l, archivo)) return;
    char *linea = (char *)malloc(BUF_LINEA);
    int n = 0, len, j;
    while (leer_linea(&l, linea, BUF_LINEA)) {
        if (linea[0] == '\0' || linea[0] == '\n' || linea[0] == '\r') continue;
        len = strlen(linea);
        while (len > 0 && (linea[len-1] == '\n' || linea[len-1] == '\r')) { linea[len-1] = '\0'; len--; }
        char *token = strtok(linea, ",");
        for (j = 0; token != NULL && j < n_features; j++) { data[n * n_features + j] = atof(token); token = strtok(NULL, ","); }
        if (token != NULL && strlen(token) < 8) strcpy(clases[n], token);
        else clases[n][0] = '\0';
        n++;
    }
    free(linea); cerrar_lector(&l);
}

int recolectar_clases(char (*train_clases)[8], int total_train, char (*clases)[8]) {
    int n = 0, i, j, existe;
    for (i = 0; i < total_train; i++) {
        if (train_clases[i][0] == '\0') continue;
        existe = 0;
        for (j = 0; j < n; j++) if (strcmp(train_clases[i], clases[j]) == 0) { existe = 1; break; }
        if (!existe && n < MAX_CLASES) { strcpy(clases[n], train_clases[i]); n++; }
    }
    return n;
}

double calcular_distancia(double *a, double *b, int n_features) {
    double sum = 0.0, d;
    int i;
    for (i = 0; i < n_features; i++) { d = a[i] - b[i]; sum += d * d; }
    return sum;
}

char *clasificar(double *train_data, char (*train_clases)[8], int total_train, double *test_punto, int k, int n_features, char (*clases)[8], int n_clases) {
    double *dist_topk = (double *)malloc(sizeof(double) * k);
    char (*clase_topk)[8] = (char (*)[8])malloc(sizeof(char[8]) * k);
    int llenos = 0, i, j, conteos[MAX_CLASES] = {0}, max_idx = 0;
    double d2;
    for (i = 0; i < total_train; i++) {
        d2 = calcular_distancia(train_data + i * n_features, test_punto, n_features);
        if (llenos < k) {
            j = llenos - 1;
            while (j >= 0 && dist_topk[j] > d2) { dist_topk[j+1] = dist_topk[j]; strcpy(clase_topk[j+1], clase_topk[j]); j--; }
            dist_topk[j+1] = d2; strcpy(clase_topk[j+1], train_clases[i]); llenos++;
        } else if (d2 < dist_topk[k-1]) {
            j = k - 2;
            while (j >= 0 && dist_topk[j] > d2) { dist_topk[j+1] = dist_topk[j]; strcpy(clase_topk[j+1], clase_topk[j]); j--; }
            dist_topk[j+1] = d2; strcpy(clase_topk[j+1], train_clases[i]);
        }
    }
    for (i = 0; i < llenos; i++) for (j = 0; j < n_clases; j++) if (strcmp(clase_topk[i], clases[j]) == 0) { conteos[j]++; break; }
    for (j = 1; j < n_clases; j++) if (conteos[j] > conteos[max_idx]) max_idx = j;
    char *resultado = (char *)malloc(8);
    strcpy(resultado, clases[max_idx]);
    free(dist_topk); free(clase_topk);
    return resultado;
}

void *knn_hilo(void *tokens) {
    Hilo_t *d = (Hilo_t *)tokens;
    int i;
    for (i = d->inicio; i < d->fin; i++)
        d->resultados[i] = clasificar(d->train_data, d->train_clases, d->total_train, d->test_data + i * d->n_features, d->k, d->n_features, d->clases, d->n_clases);
    return NULL;
}

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos) {
    int total_train = contar_filas(entrenamiento), total_test = contar_filas(prueba);
    int nf_train = contar_columnas(entrenamiento), nf_test = contar_columnas(prueba);
    if (total_train == 0 || total_test == 0 || nf_train == 0) { printf("Archivos invalidos\n"); return; }
    int n_features;
    if (nf_train < nf_test) n_features = nf_train;
    else n_features = nf_test;

    double *train_data = (double *)malloc(sizeof(double) * total_train * n_features);
    char (*train_clases)[8] = (char (*)[8])malloc(sizeof(char[8]) * total_train);
    double *test_data = (double *)malloc(sizeof(double) * total_test * n_features);
    char (*test_clases)[8] = (char (*)[8])malloc(sizeof(char[8]) * total_test);

    leer_datos(entrenamiento, train_data, train_clases, n_features);
    leer_datos(prueba, test_data, test_clases, n_features);

    char clases[MAX_CLASES][8];
    int n_clases = recolectar_clases(train_clases, total_train, clases);
    int c, i, j, r;

    printf("Train=%d Test=%d features=%d k=%d hilos=%d\n", total_train, total_test, n_features, k, hilos);
    printf("Clases (%d): ", n_clases);
    for (c = 0; c < n_clases; c++) {
        if (c == n_clases - 1) printf("%s\n", clases[c]);
        else printf("%s, ", clases[c]);
    }

    if (hilos < 1) hilos = 1;
    if (hilos > total_test) hilos = total_test;

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    char **resultados = (char **)malloc(sizeof(char *) * total_test);
    for (i = 0; i < total_test; i++) resultados[i] = NULL;

    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * hilos);
    Hilo_t *datos = (Hilo_t *)malloc(sizeof(Hilo_t) * hilos);
    int bloque = total_test / hilos, inicio = 0;

    for (i = 0; i < hilos; i++) {
        datos[i].train_data = train_data; datos[i].train_clases = train_clases;
        datos[i].total_train = total_train; datos[i].n_features = n_features;
        datos[i].test_data = test_data;
        datos[i].inicio = inicio;
        if (i == hilos - 1) datos[i].fin = total_test;
        else datos[i].fin = inicio + bloque;
        datos[i].k = k; datos[i].resultados = resultados;
        datos[i].clases = clases; datos[i].n_clases = n_clases;
        pthread_create(&threads[i], NULL, knn_hilo, (void *)&datos[i]);
        inicio = datos[i].fin;
    }
    for (i = 0; i < hilos; i++) pthread_join(threads[i], NULL);

    gettimeofday(&t1, NULL);
    double seg = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("Tiempo: %.3f s\n", seg);

    int correctos = 0, evaluados = 0, matriz[MAX_CLASES][MAX_CLASES] = {{0}};
    for (i = 0; i < total_test; i++) {
        if (test_clases[i][0] == '\0') continue;
        int idx_real = -1, idx_pred = -1;
        for (j = 0; j < n_clases; j++) {
            if (strcmp(test_clases[i], clases[j]) == 0) idx_real = j;
            if (resultados[i] && strcmp(resultados[i], clases[j]) == 0) idx_pred = j;
        }
        if (idx_real < 0) continue;
        evaluados++;
        if (idx_pred == idx_real) correctos++;
        if (idx_pred >= 0) matriz[idx_real][idx_pred]++;
    }
    double accuracy = 0.0;
    if (evaluados > 0) accuracy = (double)correctos / evaluados;

    printf("\n--- Matriz de confusion ---\n         ");
    for (c = 0; c < n_clases; c++) printf("%8s", clases[c]);
    printf("\n");
    for (r = 0; r < n_clases; r++) {
        printf("%8s ", clases[r]);
        for (c = 0; c < n_clases; c++) printf("%8d", matriz[r][c]);
        printf("\n");
    }
    printf("Accuracy: %.4f  (%d / %d)\n", accuracy, correctos, evaluados);

    int fd_m = open("metricas.csv", O_WRONLY | O_CREAT, 0644);
    if (fd_m >= 0) {
        off_t size = lseek(fd_m, 0, SEEK_END);
        char buf[256];
        int len;
        if (size == 0) {
            len = sprintf(buf, "hilos,k,total,tiempo_s,correctos,accuracy\n");
            write(fd_m, buf, len);
        }
        len = sprintf(buf, "%d,%d,%d,%.3f,%d,%.4f\n", hilos, k, evaluados, seg, correctos, accuracy);
        write(fd_m, buf, len);
        close(fd_m);
    }

    char nombre_archivo[64];
    sprintf(nombre_archivo, "resultados_h%d_k%d.csv", hilos, k);
    int fd_r = open(nombre_archivo, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_r >= 0) {
        char buf[64];
        char *clase_real, *clase_pred;
        for (i = 0; i < total_test; i++) {
            if (test_clases[i][0]) clase_real = test_clases[i];
            else clase_real = "?";
            if (resultados[i]) clase_pred = resultados[i];
            else clase_pred = "?";
            int len = sprintf(buf, "%s,%s\n", clase_real, clase_pred);
            write(fd_r, buf, len);
        }
        close(fd_r);
    }

    for (i = 0; i < total_test; i++) free(resultados[i]);
    free(resultados); free(threads); free(datos);
    free(train_data); free(train_clases); free(test_data); free(test_clases);
    printf("Terminado. Resultados en %s\n", nombre_archivo);
}
