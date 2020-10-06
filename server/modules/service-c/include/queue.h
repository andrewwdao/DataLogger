#define QUEUE_EMPTY -9999
typedef int node_data_type;

typedef struct Node_
{
    node_data_type value;
    struct Node_* next;
} Node;

typedef struct Queue_
{
    int size;
    Node* head;
    Node* tail;
} Queue;

void queue_init(Queue* queue);
void enqueue(Queue* queue, node_data_type value);
node_data_type dequeue(Queue* queue);