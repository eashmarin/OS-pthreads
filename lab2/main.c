#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void print() {
    for (int i = 0; i < 10; i++)
        printf("%d\n", i);
}

void thread_func() {
    printf("created thread:\n");
    print();
}

int main() {
    pthread_t thread;

    if (pthread_create(&thread, NULL, thread_func, NULL))
        return EXIT_FAILURE;

    if (pthread_join(thread, NULL))
        return EXIT_FAILURE;

    printf("main thread:\n");
    print();

    return 0;
}
