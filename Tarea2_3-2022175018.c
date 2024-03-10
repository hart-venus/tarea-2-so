#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
    #include <unistd.h>
#elif defined _WIN32
    #include <windows.h>
    #define sleep(x) Sleep(x * 1000)
#endif

// Definir los parámetros de los hilos
typedef struct alarm_tag {
    int seconds;
    char msg[64];
    pthread_t tid;
    struct alarm_tag *next; // puntero al siguiente struct para tenerlo como dependencia
} alarm_t;

void *alarm_thread(void *arg) {
    alarm_t *alarm = (alarm_t *)arg;

    sleep(alarm->seconds);
    printf("Hilo %s deja de esperar %d segundos\n", alarm->msg, alarm->seconds);

    if (alarm->next != NULL) { // si estoy esperando un hilo
        pthread_join(alarm->next->tid, NULL); // esperar al proximo hilo
        printf("Hilo %s para de esperar al proximo hilo\n", alarm->msg);
    } 

    free(alarm);
    return NULL;
}

int main() {
    alarm_t  *alarmaA, *alarmaB, *alarmaC;
    
    // alocar memoria para las alarmas
    alarmaA = (alarm_t *)malloc(sizeof(alarm_t));
    alarmaB = (alarm_t *)malloc(sizeof(alarm_t));
    alarmaC = (alarm_t *)malloc(sizeof(alarm_t));

    if (alarmaA == NULL || alarmaB == NULL || alarmaC == NULL) {
        perror("Error al alocar memoria\n");
        exit(1);
    }

    // configurar alarmas
    alarmaA->seconds = 10;
    strcpy(alarmaA->msg, "A");
    alarmaA->next = alarmaB;

    alarmaB->seconds = 20;
    strcpy(alarmaB->msg, "B");
    alarmaB->next = alarmaC;

    alarmaC->seconds = 30;
    strcpy(alarmaC->msg, "C");
    alarmaC->next = NULL;

    // crear hilos
    pthread_create(&alarmaA->tid, NULL, alarm_thread, alarmaA);
    pthread_create(&alarmaB->tid, NULL, alarm_thread, alarmaB);
    pthread_create(&alarmaC->tid, NULL, alarm_thread, alarmaC);

    pthread_join(alarmaA->tid, NULL); // esperar a que A finaliza 
    return 0;
}