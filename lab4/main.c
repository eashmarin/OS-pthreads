#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

void cleanup_func() {
    printf("thread exit\n");
}

void thread_func() {
    pthread_cleanup_push(cleanup_func, NULL);

    struct timespec rqtp;
    rqtp.tv_sec = 0;
    rqtp.tv_nsec = 500000000L;

    while(1) {
        printf("text\n");
        nanosleep(&rqtp, NULL);
    }

    pthread_cleanup_pop(1);
}

int main() {

    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);

    sleep(2);

    pthread_cancel(thread);

    printf("thread is cancelled\n");

    sleep(5);

    return 0;
}
