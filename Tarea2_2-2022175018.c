#include <stdio.h>
#include <string.h> 
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define MSG_SIZE 128

// definir la estructura de los mensajes
struct message {
    long type;
    char text[MSG_SIZE];
} msg; 

int main() {
    int N_MSGS = 5;
    int N_HIJOS = 2;
    pid_t pids[N_HIJOS]; // los id de mis dos hijos
    int status;
    key_t MSQKEY = 999;

    // crear cola de mensajes
    int MSQGID = msgget(MSQKEY, IPC_CREAT | S_IRUSR | S_IWUSR);

    for (int i = 0; i < N_HIJOS; i++) {
        pids[i] = fork(); // esto hace que el padre conozca todos los id de mis dos hijos
        if (pids[i] == 0) {
            for (int j = 0; j < N_MSGS; j++) {
                if (j % 2 == i) {
                    msgrcv(MSQGID, &msg, MSG_SIZE, i+1, 0);
                    printf("Hijo %d recibio: %s\n", i+1, msg.text);
                }
            }

            exit(0);
        }
    }

    // ahora, se que soy el padre (ya que no hago exit)
    int x = 1;
    for (int i = 0; i < N_MSGS; i++) {
        msg.type = x;
        sprintf(msg.text, "Mensaje no. %d", i+1);
        msgsnd(MSQGID, (void *)&msg, sizeof(msg.text), IPC_NOWAIT);
        printf("Padre envio: %s\n", msg.text);

        x = x % N_HIJOS;
        x = x + 1;
    }

    for (int i = 0; i < N_HIJOS; i++) {
        wait(&status);
    }

    msgctl(MSQKEY, IPC_RMID, NULL);
    return 0;
}