/**
 * W zadaniu mamy cztery wątki: producenta i konsumenta.
 * Każdy producent może wyprodukować zadaną z góry liczbę wartości liczby typu int, tj. 3
 * Każdy wątek producenta produkuje wartości i przechowuje je we współdzielonym buforze o długości 2, 
 * podczas gdy wątek konsumenta pobiera wartości z bufora.
 * 
 * Używamy muteksu i dwóch semaforów (full, empty) do synchronizacji dostępu do buforów. 
 * Muteks służy do zarządzania dostępem do sekcji krytycznej. 
 * Semafory umożliwiają wątkom sygnalizować sobie nawzajem, czy można wykonać operacje zapisu/odczytu.
 * 
 * Każdy wątek producenta generuje liczbą losowa, a następnie wykonuje operację na semaforze.  
 * Jeśli zapis jest możliwy to zajmuje muteks, wstawia wcześniej wygenerowaną liczbę do bufora o indeksie (na pozycji) in
 * wyświetla komunikat na ekranie 
 * (który producent wykonał operację na buforze, zawartość bufora o indeksie (na pozycji) in, wartość indeksu in)
 * inkrementuje wartość indeksu in (operacja modulo), zwalnia mutex
 * a następnie wykonuje operacje na semaforze.
 * 
 * Każdy wątek konsumenta wykonuje operacje na semaforze.
 * Jeśli odczyt jest możliwy to zajmuje muteks
 * wczytuje zawartość bufora o indeksie (z pozycji) out
 * wyświetla komunikat na ekranie
 * (który spośród trzech konsumentów wykonał operację na buforze, pobrana zawartość bufora, wartość indeksu out)
 * modyfikuje wartość indeksu out
 * Zwalnia muteks, a następnie wykonuje operacje na semaforze.
 * 
 * Funkcja main tworzy wątki producenta i konsumenta
*/


#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MaxItems 3    // liczba wartości, które producent produkuje, a konsument konsumuje
#define BufferSize 2    // wielkość bufora


sem_t empty; // sem. uzupelnic
sem_t full; // sem. uzupelnic

int in = 0;
int out = 0;
int buffer[BufferSize];                // uzupelnic

pthread_mutex_t mutex; // mutex uzupelnic


void *producer(void *arg) {
    int id = *((int *) arg);
    int item;
    for (int i = 0; i < MaxItems; i++) {
        item = rand();
        // operacja na mutex lub semafor
        // operacja na mutex lub semafor
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = item;
        printf("Producer %d: Insert Item %d at %d\n", id, buffer[in], in);
        // operacja na in
        in = (in + 1) % BufferSize;
        // operacja na mutex lub semafor
        // operacja na mutex lub semafor
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *cno) {
    int id = *((int *) cno);
    for (int i = 0; i < MaxItems; i++) {
        // operacja na mutex lub semafor
        // operacja na mutex lub semafor
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        int item = buffer[out];
        printf("Consumer %d: Remove Item %d from %d\n", id, item, out);
        // operacja na out
        out = (out + 1) % BufferSize;
        // operacja na mutex lub semafor
        // operacja na mutex lub semafor
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }

}

int main() {
    pthread_t pro[4], con[4];
    int a[4] = {1, 2, 3, 4};    // etykiety (numery) producentów i konsumentów

    pthread_mutex_init(&mutex, NULL);    // mutex
    sem_init(&empty, 0, BufferSize);    // semafor
    sem_init(&full, 0, 0);        // semafor

    for (int i = 0; i < 4; i++) {
        pthread_create(&pro[i], NULL, producer, &a[i]);
    }
    for (int i = 0; i < 4; i++) {
        pthread_create(&con[i], NULL, consumer, &a[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(pro[i], NULL);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(con[i], NULL);
    }


    pthread_mutex_destroy(&mutex);    // mutex
    sem_destroy(&empty);        // semafor
    sem_destroy(&full);            // semafor


    return 0;
}