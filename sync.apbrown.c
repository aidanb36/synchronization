//
// Created by aidan on 10/30/2022.
//

#include <pthread.h>
#include <stdio.h>
#include "sync.apbrown.h"

int main() {
    //Create struct and initialize all variables to zero
    Sync s;
    s.counter = 0;
    s.value = 0;
    s.total = 0;
    s.numRead = 0;

    //Create and Initialize Threads
    pthread_t producerThread, consumerThread;
    pthread_mutex_init(&s.counterlock, NULL);
    pthread_mutex_init(&s.readlock, NULL);

    // Open the threads
    pthread_create(&producerThread, NULL, producer, &s);
    pthread_create(&consumerThread, NULL, consumer, &s);

    //Close threads
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    return 0;
}

void* producer(Sync *s) {
    //Loop through and write N values
    for(int i = 1; i < N + 1; ++i) {
        pthread_mutex_lock(&s->counterlock);
        // while value hasn't been read yet, release lock
        // and then acquire it again to check if counter has been updated
        while (s->counter > 0) {
            pthread_mutex_unlock(&s->counterlock);
            pthread_mutex_lock(&s->counterlock);
        }
        //acquire read lock and change value
        pthread_mutex_lock(&s->readlock);
        s->value = i;
        //if we've gone through N elements, send reader -1 to stop reading
        if (i == N){
            s->value = -1;
        }
        s->counter = s->counter + 1;
        printf("producer wrote the value %d \n", (s->value));

        //release locks so consumer can read value
        pthread_mutex_unlock(&s->readlock);
        pthread_mutex_unlock(&s->counterlock);
    }
    pthread_exit(NULL);

}

void* consumer(Sync *s) {
    //acquire read lock to enter loop
    pthread_mutex_lock(&s->readlock);
    while (s->value >= 0) {
        //release read lock and check if there is something to read (counter is more than 0)
        pthread_mutex_unlock(&s->readlock);
        pthread_mutex_lock(&s->counterlock);
        //if there is something to read, read it in
        if (s->counter > 0) {
            pthread_mutex_lock(&s->readlock);
            if (s->value >= 0) {
                s->total += s->value;
                s->numRead += 1;
                printf("consumer read the value %d \n", s->value);
            }
            //modify counter to let consumer know we read the value and release read lock
            s->counter = s->counter - 1;
            pthread_mutex_unlock(&s->readlock);
        }
        pthread_mutex_unlock(&s->counterlock);
    }
    //release read lock and check if there is something to read
    pthread_mutex_unlock(&s->readlock);
    pthread_mutex_lock(&s->counterlock);
    while (s->counter < 0) {
        pthread_mutex_lock(&s->counterlock);
        pthread_mutex_unlock(&s->counterlock);
    }

    //read in value ( we know it's the negative one)
    pthread_mutex_lock(&s->readlock);
    s->numRead += 1;
    printf("consumer read the value %d \n", s->value);
    //modify counter to let consumer know we read the value and release counter lock
    s->counter = s->counter - 1;
    pthread_mutex_unlock(&s->readlock);
    pthread_mutex_unlock(&s->counterlock);

    printf("#samples = %d \n", (s->numRead));
    printf("sum = %d \n", s->total);
    pthread_exit(NULL);
}