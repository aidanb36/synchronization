//
// Created by aidan on 10/30/2022.
//

#include <pthread.h>
#include <stdio.h>
#include "sync.apbrown.h"

int main() {
    Sync s;
    s.counter = 0;
    s.value = 0;
    s.total = 0;
    s.numRead = 0;
    pthread_t producerThread, consumerThread;
    pthread_mutex_init(&s.lock, NULL);

    // Open the threads
    pthread_create(&producerThread, NULL, producer, &s);
    pthread_create(&consumerThread, NULL, consumer, &s);

    //close threads - maybe necessary
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    return 0;
}

void* producer(Sync *s) {
    for(int i = 1; i < N; ++i) {
        while (s->counter == 1) {;}
        pthread_mutex_lock(&s->lock);
        //here we need a while loop to wait for the consumer to read the value
        s->value = i;
        s->counter = s->counter + 1;
        printf("producer wrote the value %d \n", (s->value));
        //notify the consumer that it can read a value and release the lock
        pthread_mutex_unlock(&s->lock);

    }
    while (s->counter == 1) {;}
    pthread_mutex_lock(&s->lock);
    //here we need a while loop to wait for the reader to read the value
    s->value = -1;
    s->counter = s->counter + 1;
    printf("producer wrote the value %d \n", (s->value));
    //notify consumer it can read a value, and then release the lock
    pthread_mutex_unlock(&s->lock);
}

void* consumer(Sync *s) {
    //here we need a while loop to wait for the consumer to send the value
    while (s->value >= 0) {
        while (s->counter == 0 ) {;}
        pthread_mutex_lock(&s->lock);
        if (s->value >= 0){
            s->total += s->value;
        }
        s->numRead += 1;
        s->counter = s->counter - 1;
        printf("consumer read the value %d \n", s->value);
        //notify the producer that we've read the value and then release the lock
        pthread_mutex_unlock(&s->lock);

        //wait for the producer to signal they've produced another value.
        //once they do, acquire the lock
    }
    printf("#samples = %d \n", (s->numRead));
    printf("sum = %d \n", s->total);
}