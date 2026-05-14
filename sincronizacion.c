#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 150

// variables compartidas entre threads
int variables_compartidas = 0;

// mutex lock
pthread_mutex_t mutex;

void *thread_funcion(void *arg);

int main()
{
    pthread_t threads[NUM_THREADS];
    int threads_ids[NUM_THREADS];
    int i;

    // se inicializa el mutex lock
    pthread_mutex_init(&mutex, NULL);

    // se crean los hilos
    for (i = 0; i < NUM_THREADS; i++)
    {
        threads_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_funcion, &threads_ids[i]);
    }

    // esperar que los hilos terminen
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // se destruye el mutex cuando ya no es requerido
    pthread_mutex_destroy(&mutex);

    return 0;
}

// funcion a ser ejecutada por cada hilo
void *thread_funcion(void *arg)
{
    int thread_id = *((int *)arg);

    // se adquiere el mutex para que ninguno otro acceda
    pthread_mutex_lock(&mutex);

    // seccion critica
    printf("Threads %d, current value of variable %d\n", thread_id, variables_compartidas);
    variables_compartidas++;
    printf("Threads %d, new value: %d\n", thread_id, variables_compartidas);

    printf("Este mensaje puede salir nasal\n\n");

    // se libera mutex
    pthread_mutex_unlock(&mutex);
    for (int i = 1; i < 10000000; i++)
        ;
    ;
    return NULL;
}