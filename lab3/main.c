#include <stdio.h>
#include <pthread.h>

void thread_func(char* char_sequence) {
    for (int i = 0; char_sequence[i] != '\0'; i++) {
        putchar(char_sequence[i]);
    }
    printf("\n");
}

int main() {
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;

    char sequence1[] = "this sequence is printed by thread1";
    char sequence2[] = "this sequence is printed by thread2";
    char sequence3[] = "this sequence is printed by thread3";
    char sequence4[] = "this sequence is printed by thread4";

    pthread_create(&thread1, NULL, thread_func, sequence1);
    pthread_join(thread1, NULL);

    pthread_create(&thread2, NULL, thread_func, sequence2);
    pthread_join(thread2, NULL);

    pthread_create(&thread3, NULL, thread_func, sequence3);
    pthread_join(thread3, NULL);

    pthread_create(&thread4, NULL, thread_func, sequence4);
    pthread_join(thread4, NULL);

    return 0;
}
