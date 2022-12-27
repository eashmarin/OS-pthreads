#include "Node.h"

void swap(Node* node1, Node* node2) {
    Node tmp = *node1;
    *node1 = *node2;
    *node2 = tmp;
}

Node* add_element(Node* head, char* string) {
    Node* new = (Node*)malloc(sizeof(Node));

    if (new == NULL)
        return NULL;

    new->string = strdup(string);

    if (new->string == NULL) {
        free(new);
        return NULL;
    }

    new->string[strcspn(new->string, "\n")] = '\0';
    new->next = head;

    return new;
}

void print_list(Node* head) {
    printf("List: ");
    int sum = 0;
    for (Node* curr = head; curr->next != NULL; curr = curr->next) {
        printf("%s ", curr->string);
        sum += strlen(curr->string);
    }
    printf("(%d)\n", sum);
}

int calc_size(Node* head) {
    int counter = 0;

    Node* curr = head;
    while(curr->next != NULL) {
        curr = curr->next;
        counter++;
    }

    return counter;
}

void free_list(Node* head) {
    while (head->next != NULL) {
        Node* tmp = head;
        head = head->next;

        free(tmp->string);
        free(tmp);
    }
}
