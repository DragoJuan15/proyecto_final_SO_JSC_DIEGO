#ifndef KNN_H
#define KNN_H


#define MAX_CLASES 16
#define BUF_LINEA 32768
#define BUF_READ 65536

typedef struct Lector {
    int fd;
    char buffer[BUF_READ];
    int pos, len;
} Lector_t;

typedef struct Hilo {
    double *train_data, *test_data;
    char (*train_clases)[8], (*clases)[8];
    int total_train, n_features, inicio, fin, k, n_clases;
    char **resultados;
} Hilo_t;

int abrir_lector(Lector_t *l, char *archivo);

int leer_linea(Lector_t *l, char *linea, int max);

void cerrar_lector(Lector_t *l);

int contar_filas(char *archivo);

int contar_columnas(char *archivo);

void leer_datos(char *archivo, double *data, char (*clases)[8], int n_features);

int recolectar_clases(char (*train_clases)[8], int total_train, char (*clases)[8]);

double calcular_distancia(double *a, double *b, int n_features);

char *clasificar(double *train_data, char (*train_clases)[8], int total_train, double *test_punto, int k, int n_features, char (*clases)[8], int n_clases);

void *knn_hilo(void *tokens);

void knnJSC(char entrenamiento[], char prueba[], int k, int hilos);

#endif