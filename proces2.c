#include "share.h"


void zapisz_do_fifo(const char *filename) {
    if (mkfifo("wynik_fifo", 0666) == -1 && errno != EEXIST) {
        perror("błąd fifo");
        exit(1);
    }

    FILE *plik = fopen(filename, "r");
    if (!plik) {
        perror("błąd otwarcia pliku");
        exit(1);
    }

    FILE *fifo = fopen("wynik_fifo", "w");  // otwarcie FIFO do zapisu
    if (!fifo) {
        perror("błąd otwarcia fifo");
        fclose(plik);
        exit(1);
    }

    char linia[1024];
    int i = 0;
    printf("proces 2 (%d) wysyła dane\n",getpid());
    while (fgets(linia, sizeof(linia), plik)) {
        linia[strcspn(linia, "\n")] = '\0';
        size_t len = strlen(linia);
        // printf("Linia %d: %zu\n", ++i, len);

        fprintf(fifo, "Linia %d: %zu\n", ++i, len);  // to zapisuje dane dla procesu 3
    }
    printf("proces 2 (%d) wysłał dane\n",getpid());

    fclose(plik);
    fclose(fifo);
}


void proces2_main() {
    printf("proces 2: %d\n", getpid());

    int semid = semget(SEM_KEY, 1, 0666);
    if (semid == -1) {
        perror("proces2: semget");
        exit(1);
    }

    struct sembuf sem_op = {0, -1, 0};  // P (wait)
    if (semop(semid, &sem_op, 1) == -1) {
        perror("proces2: semop");
        exit(1);
    }

    int shmid = shmget(SHM_KEY, sizeof(shared_data), 0666);
    if (shmid == -1) {
        perror("proces2: shmget");
        exit(1);
    }

    shared_data *shm_ptr = (shared_data *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("proces2: shmat");
        exit(1);
    }

    zapisz_do_fifo(shm_ptr->filename);

    shmdt(shm_ptr);
    exit(0);
}
