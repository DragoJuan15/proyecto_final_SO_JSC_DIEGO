#ifndef KNN_H
#define KNN_H

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

int contar_filas(char *archivo);

int contar_columnas(char *archivo);

void leer_datos(char *archivo, double *data, char (*clases)[8], int n_features);

int recolectar_clases(char (*train_clases)[8], int total_train, char (*clases)[8]);

double calcular_distancia(double *a, double *b, int n_features);

char *clasificar(double *train_data, char (*train_clases)[8], int total_train, double *test_punto, int k, int n_features, char (*clases)[8], int n_clases);



#endif