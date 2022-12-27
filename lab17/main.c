#include "Node.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/errno.h>

#define MAX_LENGTH 80
#define MICROSECONDS_PER_SECOND 1e6

#define NOTHING 0
#define STRING_PROCEEDING 1
#define INPUT_CLOSED 2
#define NO_MEMORY 3
#define LIST_SORTING 4

pthread_mutex_t mutex;
pthread_cond_t cond;

int status = NOTHING;

void set_status(int value) {
    if (status == INPUT_CLOSED || status == NO_MEMORY) {
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
    }
    status = value;
    pthread_cond_signal(&cond);
}

void* sort_list(void* head_arg) {
    Node* head = (Node*) head_arg;
    while (1) {
        usleep(5 * MICROSECONDS_PER_SECOND);

        pthread_mutex_lock(&mutex);
        while (status == STRING_PROCEEDING) {
            pthread_cond_wait(&cond, &mutex);
        }
        set_status(LIST_SORTING);
        pthread_mutex_unlock(&mutex);

        int size = calc_size(head);
        for (int i = 0; i < size; i++) {
            Node *curr = head;
            for (int j = 0; j < size - i && curr->next->string != NULL; j++) {
                if (strcoll(curr->string, curr->next->string) > 0) {
                    char* buffer = curr->string;
                    curr->string = curr->next->string;
                    curr->next->string = buffer;
                }
                curr = curr->next;
            }
        }

        pthread_mutex_lock(&mutex);
        set_status(NOTHING);
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    char* input = (char*)malloc(sizeof(char) * MAX_LENGTH);
    Node* head = (Node*)malloc(sizeof(Node));

    pthread_t thread;
    pthread_create(&thread, NULL, sort_list, head);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    while ((input = fgets(input, MAX_LENGTH, stdin)) != NULL) {

        pthread_mutex_lock(&mutex);
        while (status != NOTHING)
            pthread_cond_wait(&cond, &mutex);
        set_status(STRING_PROCEEDING);
        pthread_mutex_unlock(&mutex);

        if (input[0] == '\n') {
            print_list(head);
        }
        else {
            Node* new = add_element(head, input);
            if (new == NULL) {
                pthread_mutex_lock(&mutex);
                set_status(NO_MEMORY);
                pthread_mutex_unlock(&mutex);

                pthread_join(thread, NULL);

                pthread_mutex_destroy(&mutex);

                free_list(head);
                free(input);

                return ENOMEM;
            }
            swap(head, new);
            head->next = new;
        }

        pthread_mutex_lock(&mutex);
        set_status(NOTHING);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    set_status(INPUT_CLOSED);
    pthread_mutex_unlock(&mutex);

    pthread_join(thread, NULL);

    pthread_mutex_destroy(&mutex);

    free_list(head);
    free(input);

    return 0;
}
