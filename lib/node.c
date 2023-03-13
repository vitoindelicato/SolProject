#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "enhanced_sc.h"


_node *create_node(long int result, char *filename) {
    _node* new_node = Malloc(sizeof(_node));
    new_node->result = result;
    new_node->filename = Malloc(strlen(filename) + 1);
    strncpy(new_node->filename, filename, strlen(filename) + 1);
    //new_node->filename = filename;
    new_node->next = NULL;
    return new_node;
}


void insert_node(_node **head, _node *new_node){
    _node *current = *head;

    if(current == NULL){
        *head = new_node;
        return;
    }

    if(current->result >= new_node->result){  /* Insertion at the beginning of the linked list*/
        new_node->next = current;
        *head = new_node;
        return;
    }

    while(current->next != NULL && current->next->result <= new_node->result){
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}



void print_list(_node *head){
    _node *current = head;
    while(current != NULL) {
        printf("%ld %s\n", current->result, current->filename);
        current = current->next;
    }
}

void free_list(_node **head){
    _node *current = *head;
    _node *next;
    while(current != NULL) {
        next = current->next;
        free(current->filename);
        free(current);
        current = next;
    }
   head = NULL;
}
