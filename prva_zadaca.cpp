#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Varijabla koja èuva trenutni broj
int trenutni_broj= 1;
// Varijabla koja èuva zadnji broj iz status.txt
int zadnji_broj= 0;

// Funkcija koja se poziva pri primitku signala
void sig_handler(int signal)
{
    if (signal == SIGUSR1)
        // Ispis trenutnog broja
        printf("Trenutni broj: %d\n", trenutni_broj);
    else if (signal == SIGTERM) {
        // Otvaranje status.txt za pisanje
        FILE* status = fopen("status.txt", "w");
        // Upis trenutnog broja u status.txt
        fprintf(status, "%d", trenutni_broj);
        fclose(status);
    }
    else if (signal == SIGINT)
        // Prekid s radom
        return;
}

int main()
{
    printf("program poceo");
    // Registriranje obrada signala
    signal(SIGUSR1, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);

    // Èitanje zadnjeg broja iz status.txt
    FILE* status = fopen("status.txt", "r");
    fscanf(status, "%d", &zadnji_broj);
    fclose(status);

    // Dodavanje izraèunatih vrijednosti u obrada.txt
    FILE* obrada = fopen("obrada.txt", "a");
    for (trenutni_broj= zadnji_broj+ 1; trenutni_broj<= 100; trenutni_broj++) {
        fprintf(obrada, "%d\n", trenutni_broj* trenutni_broj);
        //sleep(5);
    }
    fclose(obrada);

    // Ažuriranje status.txt s zadnjim brojem
    status = fopen("status.txt", "w");
    fprintf(status, "%d", trenutni_broj);
    fclose(status);
    return 0;
}

