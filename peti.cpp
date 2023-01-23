#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
  
#define N 5
#define MISLI 0
#define GLADAN 1
#define JEDE 2
#define LIJEVO (ph_num+4)%N //-npr. broj 3+4=7%5=2, a 2 je lijevo ispred 3
#define DESNO (ph_num+1)%N
  
sem_t mutex; //-zastita, ako je jedan program pokrenut, niti jedan drugi nesmije to biti
sem_t S[N]; //jedan semafor za svakog filozofa

/*deklaracija funkcija*/  
void *filozof(void *num);
void uzima_zlicu(int);
void stavlja_zlicu(int);
void test(int);
  
int state[N]; //stanje pojedinog filozofa
int phil_num[N]={0,1,2,3,4};// -jer imamo 5 filozofa
  
int main()
{
    int i;
    pthread_t thread_id[N]; //-za svakog filozofa po jedna dretva

//inicijalizacija i kreiranje mutexa(ili semafora)
//kreiranje pojedina�nog filozofa i njegovog semafora

    sem_init(&mutex,0,1); //-inicijalizacija semafora mutex, 0 je lokalni semafor, 1 je pocetna vrijednost
    for(i=0;i<N;i++) 
        sem_init(&S[i],0,0);// -nicijalizacija svakog pojedinog semafora, lokalni, 0 pocetna vrijednost
    for(i=0;i<N;i++)
    {
        pthread_create(&thread_id[i],NULL,filozof,&phil_num[i]);// -kreiranje dretve, prvi clan adresa id, NULL oznaceva dodjelu parametara(gladan, jedi,...) kako sustav oce, filozof poziva potprogram filozof, i ovo zadnje numeracija filozofa
        printf("Filozof %d misli\n",i+1); //-zato sto je deklarirano od 0, a da bi na ekranu ispisivalo od 1
    }
    for(i=0;i<N;i++)
        pthread_join(thread_id[i],NULL); //�eka terminaciju druge thread -da sve zavrse
}
  
int *filozof(void *num)
{
    while(1)
    {
        int *i = num; 
        sleep(1);
        uzima_zlicu(*i); //budi blokiran dok ne uzmes obje vilice -idemo u potprogram uzima zlicu
        sleep(0);
        stavlja_zlicu(*i); //odlozi zlice na stol - na samom kraju nakon sto smo se vratili iz uzima zlicu
    }
}
  
void uzima_zlicu(int ph_num) //Uzima zlice koje treba
{
    sem_wait(&mutex); //udji u kriticni odsjecak -ceka da mutex zavrsi, jer on moze samo jedan u jednom trenutku
    state[ph_num] = GLADAN; //zabiljezi da je filozof i gladan
    printf("Filozof %d je gladan\n",ph_num+1);
    test(ph_num); //pokusaj uzeti vilice -idemo u potprogam test
    sem_post(&mutex); //izadji iz kriticnog odsjecka
    sem_wait(&S[ph_num]); //ostani blokiran ako vilice nisu uzete - vracamo se u potprogram filozof
    sleep(1);
}
  
void test(int ph_num) //funkcija jesti
{
    if (state[ph_num] == GLADAN && state[LIJEVO] != JEDE && state[DESNO] != JEDE) //-ako trazeni filozof je gladan, a oni lijevo i desno od njega nejedu
    {
        state[ph_num] = JEDE;
        sleep(2);
        printf("Filozof %d uzima vilicu %d i %d\n",ph_num+1,LIJEVO+1,ph_num+1);
        printf("Filozof %d jede\n",ph_num+1);
        sem_post(&S[ph_num]); //-postavljamo vrijednost za trazenog filozofa i vracamo se u uzima zlicu
    }
}
  
void stavlja_zlicu(int ph_num)
{
    sem_wait(&mutex); //udji u kriticni odsjecak
    state[ph_num] = MISLI; //promijeni stanje filozofa i u razmisljanje
    printf("Filozof %d stavlja vilicu %d i %d na stol\n",ph_num+1,LIJEVO+1,ph_num+1);
    printf("Filozof %d misli\n",ph_num+1);
    test(LIJEVO); //pogledaj da li lijevi susjed moze jesti
    test(DESNO); //pogledaj da li desni susjed moze jesti
    sem_post(&mutex); //izadji iz kriticnog odsjecka
}
