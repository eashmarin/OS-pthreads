#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define num_steps 200000000
#define Context struct _CONTEXT

Context {
    int iter_number;
    int start_iter;
    double* pi_ptr;
};

void calc_pi(Context* context) {
    *context->pi_ptr = 0.0;

    for (int i = context->start_iter; i < context->iter_number ; i++) {
        *context->pi_ptr += 1.0/(i*4.0 + 1.0);
        *context->pi_ptr -= 1.0/(i*4.0 + 3.0);
    }

    pthread_exit(context->pi_ptr);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("invalid argument - pass amount of threads as a parameter");
        return (EXIT_FAILURE);
    }

    clock_t start = clock();

    double pi = 0;
    double* partial_sum = (double*)malloc(sizeof(double));
    int threads_number = atoi(argv[1]);

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * threads_number);

    Context* contexts = (Context*)malloc(sizeof(Context) * threads_number);

    int steps_per_thread = num_steps / threads_number;

    for (int i = 0; i < threads_number; i++) {
        contexts[i].pi_ptr = (double*)malloc(sizeof(double));
        contexts[i].iter_number = steps_per_thread;
        contexts[i].start_iter = i * steps_per_thread;
    }

    for (int i = 0; i < threads_number; i++) {
        int error;
        if ((error = pthread_create(&threads[i], NULL, calc_pi, &contexts[i])) != 0) {
            printf("ERROR while creating thread: %d", error);
        }
    }

    for (int i = 0; i < threads_number; i++) {
        pthread_join(threads[i], &partial_sum);
        pi += *partial_sum * 4.0;
    }

    printf("time elapsed = %lu (milliseconds)\n", (clock() - start) * 1000 / CLOCKS_PER_SEC);
    printf("pi done - %.15g \n", pi);

    free(threads);
    free(contexts);

    return (EXIT_SUCCESS);
}
