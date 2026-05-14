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

void knnJSC(char training_csv[], char test_csv[], int k, int num_hilos);

void cargar_csv(char nombre[], Data_t datos[],int *n);

float distancia_euclideana(Data_t a, Data_t b);

int knn(Data_t train[], int train_n, Data_t test, int k);

void ordenar_vecinos(Vecino_t vecinos[], int n);

void *knnJSC_thread(void *hilo_void);

void guardar_resultados(char nombre[], Data_t test[], int n,float accuracy);

#endif