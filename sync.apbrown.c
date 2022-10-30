//
// Created by aidan on 10/30/2022.
//

#include <pthread.h>
#include <stdio.h>
#include "sync.apbrown.h"

int main() {
    Sync s;
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
    pthread_mutex_lock(&s->lock);
    if (s->counter < N-1) {
        for(int i = 0; i<=N-1; ++i) {
            s->value = i;
            s->counter = s->counter + 1;
            printf("producer wrote the value %d \n", (s->value));
        }
    }
    else{
        s->value = -1;
    }
    pthread_mutex_unlock(&s->lock);
}

void* consumer(Sync *s) {
    pthread_mutex_lock(&s->lock);
    if (s->counter > 0 && s->value != -1) {
        s->total += s->value;
        s->counter = s->counter - 1;
    }
    printf("#samples = %d \n", (s->counter));
    printf("sum = %d \n", s->total);
    pthread_mutex_unlock(&s->lock);

}