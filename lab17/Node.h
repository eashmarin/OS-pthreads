#ifndef LAB17_NODE_H
#define LAB17_NODE_H

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#define Node struct NODE

Node {
    Node* next;
    char* string;
};

void swap(Node* node1, Node* node2);

Node* add_element(Node* head, char* string);

void print_list(Node* head);

int calc_size(Node* head);

void free_list(Node* head);

#endif //LAB17_NODE_H
