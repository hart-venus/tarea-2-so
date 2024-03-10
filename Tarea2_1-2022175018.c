#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    key_t SHM_KEY = 999;
    int INT_ARR_SIZE = 20;
    int SHM_SIZE = INT_ARR_SIZE * sizeof(int);

    // Crear el segmento de memoria compartida
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (shm_id < 0)
    {
        perror("Error al crear el segmento de memoria compartida\n");
        exit(1);
    }

    // Asignar el segmento de memoria compartida 
    int *shm_arr = (int *)shmat(shm_id, NULL, 0);
    if (shm_arr == (void *)-1)
    {
        perror("Error al asignar el segmento de memoria compartida\n");
        exit(1);
    }

    // inicializar el arreglo con valores aleatorios del 1-100
    for (int i = 0; i < INT_ARR_SIZE; i++)
    {
        shm_arr[i] = rand() % 100 + 1;
    }

    for (int i = 0; i < INT_ARR_SIZE / 2; i++) {
        // crear 10 procesos hijos
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error al crear el proceso hijo\n");
            exit(1);
        }

        if (pid == 0) { // solo los hijos ejecutan esta rama
            printf("Hijo %d: %d + %d = %d\n", getpid(), shm_arr[i*2], shm_arr[i*2 + 1], shm_arr[i*2] + shm_arr[i*2 + 1]);
            shmdt(shm_arr); // desasignar el segmento de memoria compartida
            exit(0);
        }
    }

    // esperar a mis procesos hijos
    wait(NULL);

    // separar el segmento de memoria compartida
    shmdt(shm_arr);
    shmctl(shm_id, IPC_RMID, NULL); // borrar el segmento de memoria compartida
    return 0;
}
