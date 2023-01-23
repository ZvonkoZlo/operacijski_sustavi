#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
// Varijabla koja oznaèava da li nit želi uæi u kritièni odsjeèak
int nit1_zastava = 0;
int nit2_zastava = 0;
// Varijabla koja oznaèava koja nit može uæi u kritièni odsjeèak
int pravo = 1;

void* nit1_func(void* args) {
    for (int k = 1; k <= 5; k++) {
        nit1_zastava = 1;
        while (nit2_zastava && pravo == 2);
        // Kritièni odsjeèak
        for (int m = 1; m <= 5; m++) {
            printf("Nit 1, k = %d, m = %d\n", k, m);
            
        }
        nit1_zastava = 0;
        pravo = 2;
    }
    return NULL;
}

void* nit2_func(void* args) {
    for (int k = 1; k <= 5; k++) {
        nit2_zastava = 1;
        while (nit1_zastava && pravo == 1);
        // Kritièni odsjeèak
        for (int m = 1; m <= 5; m++) {
            printf("Nit 2, k = %d, m = %d\n", k, m);
            
        }
        nit2_zastava= 0;
        pravo = 1;
    }
    return NULL;
}

int main() {
    pthread_t nit1, nit2;
    pthread_create(&nit1, NULL, nit1_func, NULL);
    pthread_create(&nit2, NULL, nit2_func, NULL);
    pthread_join(nit1, NULL);
    pthread_join(nit2, NULL);
    return 0;
}

