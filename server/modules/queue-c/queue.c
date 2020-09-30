#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void queue_init(Queue* q)
{
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
}

void enqueue(Queue* q, node_data_type value)
{
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->next = NULL;

    if (q->size)
        q->tail->next = new_node;
    else
        q->head = new_node;
    
    q->tail = new_node;
    ++(q->size);
}

node_data_type dequeue(Queue* q)
{
    if (q->size)
    {
        node_data_type res = q->head->value;
        q->head = q->head->next;
        --(q->size);

        return res;
    }
    return QUEUE_EMPTY;
}
