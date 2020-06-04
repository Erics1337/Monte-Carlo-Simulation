//Eric Swanson cs370 hw13
//Multi-threaded monte carlo simulation pi estimator
//using synchronization

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <math.h>
#include <time.h>
using namespace std;

unsigned long long hits = 0;
unsigned long long attempts = 0;
long Niterations;
long Nthreads;
long Ndelay;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;


void *run(void *arg){
    long hitsThread = 0;
    long attemptsThread = 0;
    for (long i = 0; i < Niterations; i++) {
        double x = drand48();
        double y = drand48();
        
        if ((x * x) + (y * y) < 1) {
            hitsThread++;
        }
        attemptsThread++;
    }
    pthread_mutex_lock(&lock1);
    hits += hitsThread;
    attempts += attemptsThread;
    pthread_mutex_unlock(&lock1);
    
    return 0;
}


void *display(void *arg){
    while(true){
        usleep((int)Ndelay);
        pthread_mutex_lock(&lock1);
        printf("Pi: %.10f\n", (4.0 * hits) / attempts);
        pthread_mutex_unlock(&lock1);
    }
    return 0;
}


int main(int argc, char **argv){
    if (argc == 4){
        //read option variable values and error check
        char* endptr;
        Niterations = strtol(argv[1], &endptr, 10);
        if (*endptr != '\0')
            fprintf(stderr, "invalid argument `%s' is not numeric\n", argv[1]);
        Nthreads = strtol(argv[2], &endptr, 10);
        if (*endptr != '\0')
            fprintf(stderr, "invalid argument `%s' is not numeric\n", argv[1]);
        Ndelay = strtol(argv[3], &endptr, 10);
        if (*endptr != '\0')
            fprintf(stderr, "invalid argument `%s' is not valid\n", argv[1]);
        
        
        //seed random
        srand((unsigned)time(NULL));
        
        //allocate mem for worker threads
        pthread_t *wThreads = (pthread_t *)malloc(Nthreads * sizeof(pthread_t));
        if (wThreads == NULL)
            perror("error allocating threads");
        //create worker threads
        for (int i = 0; i < Nthreads; i++) {
            if (pthread_create(&wThreads[i], NULL, run, (void *) NULL) != 0)
                perror("error creating threads");
        }
        
        //allocate display thread
        pthread_t dThread;
        
        //create display thread
        if (pthread_create(&dThread, NULL, display, (void *) NULL) != 0)
            perror("error creating thread");
        
        //join threads
        for (int i = 0; i < Nthreads; i++) {
            if (pthread_join(wThreads[i], NULL) != 0)
                perror("error joining threads");
        }
        //exit display thread
        pthread_cancel(dThread);
        
        
        //free mem
        pthread_mutex_destroy(&lock1);
        free(wThreads);
        
        //display final results
        printf("Final estimate of Pi: %.10f\n", (4.0 * hits) / attempts);
        printf("Total hits: %llu\n", hits);
        printf("Total attempts: %llu\n", attempts);
        
        return 0;
    }
    
    else
        printf("usage: ./hw13 Niterations Nthreads Ndelay\n");
}
