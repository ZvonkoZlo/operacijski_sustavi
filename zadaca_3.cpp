#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define true 1
#define false 0

int n_threads; // Broj dretvi
int n_tables; // Broj stolova
int tables[100]; // Polje koji predstavlja stolove
int lamport_timestamp[100]; // Polje koji predstavlja Lamportov timestamp

void* reservation(void* arg) {
    int id = *((int*) arg);
    while (true) {
        sleep(1); // Èekaj jednu sekundu
        int table = rand() % n_tables; // Slucajno odaberi stol
        int max_timestamp = 0;

        // Uði u kritièni odsjeèak - provjeri stanje stola
        for (int i = 0; i < n_threads; i++) {
            if (lamport_timestamp[i] > max_timestamp) {
                max_timestamp = lamport_timestamp[i];
            }
        }
        lamport_timestamp[id] = max_timestamp + 1;
        if (tables[table] == -1) {
            tables[table] = id;
            printf("Dretva %d: rezerviram stol %d, stanje: ", id, table);
            for (int i = 0; i < n_tables; i++) {
                if (tables[i] == -1) {
                    printf("-");
                } else {
                    printf("%d", tables[i]);
                }
            }
            printf("\n");
            break;
        } else {
            printf("Dretva %d: neuspjela rezervacija stola %d, stanje: ", id, table);
            for (int i = 0; i < n_tables; i++) {
                if (tables[i] == -1) {
                    printf("-");
                } else {
                    printf("%d", tables[i]);
                }
            }
            printf("\n");
        }
    }
    return NULL;
}

int main() {
  

    n_threads = 5;//5 dretvi proizvoljne vrijednosti 
    n_tables =5;//5 stolova

    for (int i = 0; i < n_tables; i++) {
        tables[i] = -1; // Inicijaliziraj sve stolove kao slobodne
    }

    pthread_t threads[n_threads];
    int thread_ids[n_threads];

    for (int i = 0; i < n_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, reservation, &thread_ids[i]);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

