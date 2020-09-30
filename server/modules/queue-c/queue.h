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

void queue_init(Queue*);
void enqueue(Queue*, node_data_type);
node_data_type dequeue(Queue*);