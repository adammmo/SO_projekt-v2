#include "share.h"


void proces3_main() {
    printf("proces 3: %d\n", getpid());
    // Otwiera FIFO do odczytu
    FILE *fifo = fopen("wynik_fifo", "r");
    if (!fifo) {
        printf("błąd odczytu fifo");
        exit(1);
    }

    char linia[256];
    printf("proces 3 (%d) odczytuje dane\n",getpid());
    while (fgets(linia, sizeof(linia), fifo)) {
        printf("%s", linia);
    }

    fclose(fifo);
    printf("proces3 (%d): zakończył odczyt\n",getpid());
    exit(0);
}