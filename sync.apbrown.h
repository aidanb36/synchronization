//
// Created by aidan on 10/30/2022.
//
#ifndef SYNCHRONIZATION_SYNC_APBROWN_H
#define SYNCHRONIZATION_SYNC_APBROWN_H

#include <pthread.h>

typedef struct {
    int value;
    int counter;
    int total;
    int numRead;
    pthread_mutex_t counterlock;
    pthread_mutex_t readlock;
} Sync;

#define N 10

void* producer(Sync *s);
void* consumer(Sync *s);

#endif //SYNCHRONIZATION_SYNC_APBROWN_H
