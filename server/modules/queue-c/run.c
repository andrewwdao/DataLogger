#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int main()
{
    Queue q;
    queue_init(&q);
    
    while (1)
    {
        Node* head = q.head;
        printf("Queue: ");
        while (head != NULL)
        {
            printf("%d -> ", head->value);
            head = head->next;
        }
        printf("NULL");

        printf("\n\t1. Enqueue\n\t2. Dequeue\nChoose: "); int choice;
        scanf("%d", &choice);

        if (choice == 1)
        {
            printf("Enter value: ");
            node_data_type val; scanf("%d", &val);
            enqueue(&q, val);
        }
        else if (choice == 2)
        {
            node_data_type data = dequeue(&q);
            printf("result: %d\n", data);
        }

        printf("\n");
    }


    return 0;
}