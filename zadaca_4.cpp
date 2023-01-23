#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5 // Broj sjedala

sem_t empty_seats; // Semafor za prazna sjedala
sem_t full_seats; // Semafor za puna sjedala
sem_t ride_over; // Semafor za signaliziranje kraja vo�nje
sem_t mutex; // Semafor za me�usobno isklju�ivanje

int seats[N]; // Polje koje predstavlja sjedala
int count = 0; // Broj ljudi na vo�nji

void* visitor(void* arg) {
    int id = *((int*) arg);
    printf("Posjetitelj %d: Stigao na atrakciju\n", id);
    sem_wait(&empty_seats); // �ekaj na prazno sjedalo
    sem_wait(&mutex); // U�i u kriti�ni odsje�ak
    seats[count] = id;
    count++;
    printf("Posjetitelj %d: usao na vrtuljak \n", id);
    sem_post(&mutex); // Izadi iz kriti�nog odsje�ka
    sem_post(&full_seats); // Signaliziraj da je atrakcija puna
    sem_wait(&ride_over); // �ekaj na kraj vo�nje
    printf("Posjetitelj %d: sisao s vrtuljka\n", id);
    return NULL;
}

void* ferris_wheel(void* arg) {
    while (1) {
        sem_wait(&full_seats); // �ekaj dok atrakcija nije puna
        printf("Vrtuljak: Pokrenut\n");
        sem_wait(&mutex); // U�i u kriti�ni odsje�ak
        for (int i = 0; i < count; i++) {
            printf("Vrtuljak: Posjetitelj %d na vo�nji\n", seats[i]);
        }
        sem_post(&mutex); // Izadi iz kriti�nog odsje�ka
        printf("Vrtuljak: Zavr�en\n");
        sem_post(&ride_over); // Signaliziraj kraj vo�nje
        sem_wait(&mutex); // U�i u kriti�ni odsje�ak
        count = 0; // O�isti sjedala
        for (int i = 0; i < N; i++) {
            seats[i] = -1;
        }
        sem_post(&empty_seats); // Signaliziraj prazna sjedala
        sem_post(&mutex); // Izadi iz kriti�nog odsje�ka
    }
    return NULL;
}
int main() {
    // Inicijaliziraj semafore
    sem_init(&empty_seats, 0, N);
    sem_init(&full_seats, 0, 0);
    sem_init(&ride_over, 0, 0);
    sem_init(&mutex, 0, 1);

    // Inicijaliziraj sjedala
    for (int i = 0; i < N; i++) {
        seats[i] = -1;
    }

    // Stvori dretve
    pthread_t ferris_wheel_thread;
    pthread_t visitor_threads[N];
    int visitor_ids[N];

    pthread_create(&ferris_wheel_thread, NULL, ferris_wheel, NULL);

    for (int i = 0; i < N; i++) {
        visitor_ids[i] = i;
        pthread_create(&visitor_threads[i], NULL, visitor, &visitor_ids[i]);
    }

    // Spoji dretve
    pthread_join(ferris_wheel_thread, NULL);
    for (int i = 0; i < N; i++) {
        pthread_join(visitor_threads[i], NULL);
    }

    // Unisti semafore
    sem_destroy(&empty_seats);
    sem_destroy(&full_seats);
    sem_destroy(&ride_over);
    sem_destroy(&mutex);

    return 0;
}



