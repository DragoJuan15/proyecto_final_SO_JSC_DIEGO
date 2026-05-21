#ifndef KNN_H
#define KNN_H

int contar_filas(char *archivo);

int contar_columnas(char *archivo);

void leer_datos(char *archivo, double *data, char (*clases)[8], int n_features);

int recolectar_clases(char (*train_clases)[8], int total_train, char (*clases)[8]);

double calcular_distancia(double *a, double *b, int n_features);

char *clasificar(double *train_data, char (*train_clases)[8], int total_train, double *test_punto, int k, int n_features, char (*clases)[8], int n_clases);

void *knn_hilo(void *arg);

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos);

#endif