#include "share.h"

int main() {
    pid_t pid;

    printf("proces macierzysty: %d\n", getpid());

    // Tworzenie semafora
    int semid = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        if (errno == EEXIST) {
            semid = semget(SEM_KEY, 1, 0);  // otwarcie istniejącego
            if (semid == -1) {
                perror("main: semget (otwarcie istniejącego)");
                exit(1);
            }
        } else {
            perror("main: semget (tworzenie)");
            exit(1);
        }
    } else {
        // jeśli nowo utworzony — ustaw wartość na 0
        if (semctl(semid, 0, SETVAL, 0) == -1) {
            perror("main: semctl SETVAL");
            exit(1);
        }
    }

    // Tworzenie procesu potomnego 1
    switch (pid = fork()) {
        case 0:
            proces1_main();
            break;
        case -1:
            perror("błąd forka proces1");
            exit(1);
        default:
            break;
    }

    // Tworzenie procesu potomnego 2
    switch (pid = fork()) {
        case 0:
            proces2_main();
            break;
        case -1:
            perror("błąd forka proces2");
            exit(1);
        default:
            break;
    }

    // Tworzenie procesu potomnego 3
    switch (pid = fork()) {
        case 0:
            proces3_main();
            break;
        case -1:
            perror("błąd forka proces3");
            exit(1);
        default:
            break;
    }

    // Oczekiwanie na zakończenie procesów potomnych
    for (int i = 0; wait(NULL) > 0; i++) {
        printf("koniec procesu %d\n",i+1);
    }

    return 0;
}
