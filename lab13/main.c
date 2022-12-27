#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define STRINGS_NUMBER 10

pthread_mutex_t mutex;
pthread_cond_t cond;

bool is_child_turn;


void* thread_func() {
    for (int i = 0; i < STRINGS_NUMBER; i++) {
        pthread_mutex_lock(&mutex);
        
        while (!is_child_turn) {
            pthread_cond_wait(&cond, &mutex);
        }

        printf("child thread: %d\n", i);

        is_child_turn = false;
        pthread_cond_signal(&cond);
        
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    if (pthread_create(&thread, NULL, thread_func, NULL))
        return EXIT_FAILURE;

    for (int i = 0; i < STRINGS_NUMBER; i++) {
        pthread_mutex_lock(&mutex);
        
        printf("parent thread: %d\n", i);

        is_child_turn = true;
        pthread_cond_signal(&cond);
        
        while (is_child_turn) {
            pthread_cond_wait(&cond, &mutex);
        }
        
        pthread_mutex_unlock(&mutex);
    }

    if (pthread_join(thread, NULL))
        return EXIT_FAILURE;

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
