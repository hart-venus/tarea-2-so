#include <errno.h> 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_THREADS 10 

pthread_t tids[N_THREADS];

void handle_error_en(int en, const char *msg) {
    errno = en;
    perror(msg);
    exit(EXIT_FAILURE); // 1
}

void *thread_func(void *arg) {
    int thread_num = *((int *)arg);
    free(arg); // liberar la memoria asignada originalmente para manejarlo solamente en el hilo

    // configurar cancelabilidad asincrona
    int s = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_setcanceltype");
    }

    // mientras yo este vivo
    while (1) {
        printf("Hilo %d: Sigo vivo\n", thread_num);
        sleep(1);
        // tratar de cancelar a un hilo aleatorio 
        int random = rand() % N_THREADS;
        if (pthread_self() != tids[random]) {
            pthread_cancel(tids[random]);
            printf("Hilo %d: Cancelo el hilo %d\n", thread_num, tids[random]);
        }
    }

    return NULL;
}

int main(){
    int s; 
    // ciclo de creado de hilos
    for (int i = 0; i < N_THREADS; i++) {
        int *arg = malloc(sizeof(int)); // alocar un int como argumento
        if (arg == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        *arg = i+1; // asignar el valor de i al argumento
        s = pthread_create(&tids[i], NULL, thread_func, arg);
        if (s != 0) {
            handle_error_en(s, "pthread_create");
        }
    }

    // ciclo de espera 
    for (int i = 0; i < N_THREADS; i++) {
        void *res; 
        s = pthread_join(tids[i], &res);
        if (s != 0) {
            handle_error_en(s, "pthread_join");
        }
        if (res == PTHREAD_CANCELED) {
            printf("Reporte principal: El hilo %d fue cancelado (%d) \n", i+1, tids[i]);
        }
    }
    return 0;
}