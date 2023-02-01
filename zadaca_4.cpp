#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//Modelirati ringi¨pil  s dva tipa dretvi/procesa:dretvama posjetitelji koji zele na voznju i vrtuljak.Napisati program koji nece dozviliti posjetiteljima da se ukrcaju kada vise nema mjesta i da vrtuljak pokrenemo tek kada je pun.Za sinkronizaciju korisiti opce semafore i dodatne varijable.

#define N 5 // Broj sjedala

sem_t prazna_sjedala; // Semafor za prazna sjedala
sem_t zauzeta_sjedala; // Semafor za puna sjedala
sem_t voznja_gotova; // Semafor za signaliziranje kraja voznje
sem_t mutex; // Semafor za meðusobno iskljucivanje

int seats[N]; // Polje koje predstavlja sjedala
int count = 0; // Broj ljudi na voznji

void* posjetitelj(void* arg) {
    int id = *((int*) arg);
    printf("Posjetitelj %d: Stigao na atrakciju\n", id);
    sem_wait(&prazna_sjedala); // cekaj na prazno sjedalo
    sem_wait(&mutex); // Uði u kriticni odsjecak
    seats[count] = id;
    count++;
    printf("Posjetitelj %d: usao na vrtuljak \n", id);
    sem_post(&mutex); // Izadi iz kriticnog odsjecka
    sem_post(&zauzeta_sjedala); // Signaliziraj da je atrakcija puna
    sem_wait(&voznja_gotova); // cekaj na kraj voznje
    printf("Posjetitelj %d: sisao s vrtuljka\n", id);
    return NULL;
}

void* vrtuljak(void* arg) {
    while (1) {
        sem_wait(&zauzeta_sjedala); // cekaj dok atrakcija nije puna
        printf("Vrtuljak: Pokrenut\n");
        sem_wait(&mutex); // Uði u kriticni odsjecak
        for (int i = 0; i < count; i++) {
            printf("Vrtuljak: Posjetitelj %d na voznji\n", seats[i]);
        }
        sem_post(&mutex); // Izadi iz kritiènog odsjeèka
        printf("Vrtuljak: Zavrsen\n");
        sem_post(&voznja_gotova); // Signaliziraj kraj voznje
        sem_wait(&mutex); // Uði u kriticni odsjecak
        count = 0; // Ocisti sjedala
        for (int i = 0; i < N; i++) {
            seats[i] = -1;
        }
        sem_post(&prazna_sjedala); // Signaliziraj prazna sjedala
        sem_post(&mutex); // Izadi iz kriticnog odsjecka
    }
    return NULL;
}
int main() {
    // Inicijaliziraj semafore
    sem_init(&prazna_sjedala, 0, N);
    sem_init(&zauzeta_sjedala, 0, 0);
    sem_init(&voznja_gotova, 0, 0);
    sem_init(&mutex, 0, 1);

    // Inicijaliziraj sjedala
    for (int i = 0; i < N; i++) {
        seats[i] = -1;
    }

    // Stvori dretve
    pthread_t vrtuljak_thread;
    pthread_t posjetitelj_threads[N];
    int posjetitelj_ids[N];

    pthread_create(&vrtuljak_thread, NULL, vrtuljak, NULL);

    for (int i = 0; i < N; i++) {
        posjetitelj_ids[i] = i;
        pthread_create(&posjetitelj_threads[i], NULL, posjetitelj, &posjetitelj_ids[i]);
    }

    // Spoji dretve
    pthread_join(vrtuljak_thread, NULL);
    for (int i = 0; i < N; i++) {
        pthread_join(posjetitelj_threads[i], NULL);
    }
	//Skup semafora treba unistiti nakon upotrebe jer u protivnom ostaje kao 
	//trajno zauzeto sresdstvo u sustavu
    // Unisti semafore
    sem_destroy(&prazna_sjedala);
    sem_destroy(&zauzeta_sjedala);
    sem_destroy(&voznja_gotova);
    sem_destroy(&mutex);

    return 0;
}



