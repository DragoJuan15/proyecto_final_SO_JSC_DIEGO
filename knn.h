#ifndef KNN_H
#define KNN_H

#include <pthread.h>

typedef struct Data
{
    float x;
    float y;

    int clase_real;
    int clase_predicha;

} Data_t;

typedef struct Vecino
{
    float distancia;
    int clase;

} Vecino_t;

typedef struct HiloKNN
{
    Data_t *train;
    Data_t *test;

    int train_n;

    int inicio;
    int fin;

    int k;

} HiloKNN_t;

double distancia_euclideana(double x1, double y1, double x2, double y2);

void cargar_training();

void cargar_test();

void ordenar(double distancias[], char clases[][50], int n);

char *predecir(double x, double y, int k);

void *knn_thread(void *tokens);

void guardar_resultados(int k, int num_hilos);

void knnJSC(int k, int num_hilos);

#endif