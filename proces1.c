#include "share.h"

int licz = 0;
int pojemnosc = 4;
char **dane = NULL;
int x;

void collect_data_plik() {
    char nazwa[256];
    printf("podaj nazwe pliku: \n");
    scanf("%s", nazwa);
    printf("Pobieram dane z pliku\n");

    FILE *plik;
    char linia[1024];
    plik = fopen(nazwa, "r");
    if (plik == NULL) {
        printf("Błąd otwarcia pliku\n");
        exit(1);
    }

    dane = malloc(pojemnosc * sizeof(char*));
    if (!dane) {
        printf("błąd alokacji pamięci");
        exit(1);
    }

    while (fgets(linia, sizeof(linia), plik)) {
        linia[strcspn(linia, "\n")] = '\0';

        if (licz >= pojemnosc) {
            pojemnosc *= 2;
            dane = realloc(dane, pojemnosc * sizeof(char*));
            if (!dane) {
                printf("błąd alokacji pamięci");
                exit(1);
            }
        }

        dane[licz] = malloc(strlen(linia) + 1);
        strcpy(dane[licz], linia);
        licz++;
    }

    fclose(plik);
}

void collect_data_klaw() {
    int ile;
    printf("Ile linii tekstu chcesz wprowadzić? ");
    scanf("%d", &ile);
    getchar();

    dane = malloc(ile * sizeof(char *));
    if (!dane) {
        printf("Błąd alokacji pamięci\n");
        exit(1);
    }

    for (int i = 0; i < ile; i++) {
        char bufor[1024];
        printf("Linia %d: ", i + 1);
        fgets(bufor, sizeof(bufor), stdin);

        size_t len = strlen(bufor);
        if (len > 0 && bufor[len - 1] == '\n') {
            bufor[len - 1] = '\0';
            len--;
        }

        dane[i] = malloc((len + 1) * sizeof(char));
        if (!dane[i]) {
            printf("Błąd alokacji pamięci\n");
            exit(1);
        }
        strcpy(dane[i], bufor);
    }

    licz = ile;
}

void print_dane() {
    for (int i = 0; i < licz; i++) {
        printf("linia %d: %s\n", i + 1, dane[i]);
    }
}


void proces1_main() {
    printf("Uruchomiono proces 1. PID: %d\n", getpid());
    printf("1 - pobranie danych z pliku\n2 - pobranie danych z klawiatury\nW jaki sposób pobrać dane: ");
    scanf("%d", &x);

    if (x == 1) collect_data_plik();
    else collect_data_klaw();
    //print_dane();
    FILE *out = fopen("dane.txt", "w");
    if (!out) {
        perror("fopen dane.txt");
        exit(1);
    }

    for (int i = 0; i < licz; i++) {
        fprintf(out, "%s\n", dane[i]);
        free(dane[i]);
    }
    fclose(out);
    free(dane);
    printf("proces 1 (%d) tworzy segment pamięci współdzielonej\n", getpid());
    int shmid = shmget(SHM_KEY, sizeof(shared_data), IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget"); exit(1); }

    printf("proces 1 (%d) przyłącza pamięć współdzieloną\n", getpid());
    shared_data *shm_ptr = (shared_data *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) { perror("shmat"); exit(1); }

    printf("proces 1 (%d) zapisuje nazwę pliku i liczbę linii do pamięci współdzielonej\n", getpid());
    strncpy(shm_ptr->filename, "dane.txt", sizeof(shm_ptr->filename));
    shm_ptr->liczba_linii = licz;

    printf("proces 1 (%d) otwiera semafor\n", getpid());
    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget"); exit(1); }

    printf("proces 1 (%d) podnosi semafor (V)\n", getpid());
    struct sembuf sem_op = {0, 1, 0};
    if (semop(semid, &sem_op, 1) == -1) { perror("semop"); exit(1); }

    shmdt(shm_ptr);
    exit(0);
}
