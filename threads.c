#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct FILE_DATA_STRUCT
{
    FILE *fd;
    int id;
    float result;
} FILE_DATA;

void *lectura_archivo_thread(void *file_data);
float lectura_sin_thread(FILE *fd);

int main(void){
    clock_t t;
    double time_taken;
    FILE_DATA file1, file2;
    FILE *fd_sem1, *fd_sem2;
    pthread_t threadf1, threadf2;
    fd_sem1 = fopen("Semestre1","r");
    fd_sem2 = fopen("Semestre2","r");
    file1.fd = fd_sem1;
    file2.fd = fd_sem2;
    file1.id = 1;
    file2.id = 2;
    file1.result = 0;
    file2.result = 0;
    t = clock();
    pthread_create(&threadf1, NULL, lectura_archivo_thread, (void *) &file1);
    pthread_create(&threadf2, NULL, lectura_archivo_thread, (void *) &file2);

    pthread_join(threadf1, NULL);
    pthread_join(threadf2, NULL);

    printf("Promedio Archivo 1: %f\n", file1.result);
    printf("Promedio Archivo 2: %f\n", file2.result);

    t = clock() - t;

    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Tiempo transcurrido con hilos: %f\n", time_taken);

    fclose(fd_sem1);
    fclose(fd_sem2);

    fd_sem1 = fopen("Semestre1","r");
    fd_sem2 = fopen("Semestre2","r");

    t = clock();

    printf("Promedio Archivo 1: %f\n", lectura_sin_thread(fd_sem1));
    printf("Promedio Archivo 2: %f\n", lectura_sin_thread(fd_sem2));

    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Tiempo transcurrido sin hilos: %f\n", time_taken);
}

void *lectura_archivo_thread(void *file_data){
    float num;
    float acumulador = 0;
    int cantidad = 0;
    FILE_DATA * passed_data = file_data;
    while (fscanf(passed_data->fd, "%f", &num) != EOF){
        acumulador += num;
        cantidad++;
    }
    passed_data -> result = acumulador / cantidad;
    return NULL;
}

float lectura_sin_thread(FILE *fd){
    float num;
    float acumulador = 0;
    int cantidad = 0;
    while (fscanf(fd, "%f", &num) != EOF){
        acumulador += num;
        cantidad++;
    }
    return acumulador / cantidad;
}