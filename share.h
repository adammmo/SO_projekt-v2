// użyte biblioteki
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <errno.h> 

#ifndef SHARE_H
#define SHARE_H

#define MAX 2048     // maksymalna liczba linii
#define DL 512      // maksymalna długość linii
#define SHM_KEY 1234 // klucz do pamięci współdzielonej

// struktura do współdzielenia danych między procesami
typedef struct {
    char filename[256];
    int liczba_linii;
} shared_data;
;
#define SHM_SIZE sizeof(shared_data)

// procesy potomne
void proces1_main();
void proces2_main();
void proces3_main();

// funkcje współdzielone
void collect_data_plik();
void collect_data_klaw();

#endif

#define SEM_KEY 5678