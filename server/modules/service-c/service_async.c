#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <queue.h>
#include <mqtt.h>
#include <pgsql.h>

// PGSQL params
#define CONN_INFO "dbname=test host=datalogger.ddns.net user=postgres password=admin123"

// MQTT params
#define ADDRESS     "tcp://datalogger.ddns.net:1883"
#define CLIENT_SID  "ClientSub"
#define TOPIC_SUB   "data"
#define QOS         2

MQTTClient* pub_client;
MQTTClient* sub_client;

Queue queue;
pthread_t process_message_tid;
volatile int message_processing;
volatile int cnt = 0;
volatile int qcnt = 0;


void *process_message(void *vargp)
{
    while (1)
    {
        if (!queue.size) continue;

        char* payload = dequeue(&queue);
        
        printf("QUEUE PAYLOAD: ");
        printf("%s\n", payload);

        char* timestamp = payload;
        char* id = strstr(payload, ":");
        if (id == NULL)
        {
            printf("Payload format is not correct! Aborted!\n");
            free(payload);
            continue;
        }
        else *(id++) = '\0';

        char* data = strstr(id, ":");
        if (id == NULL)
        {
            printf("Payload format is not correct! Aborted!\n");
            free(payload);
            continue;
        }
        else *(data++) = '\0';

        char json_with_quote[strlen(data) + 5];
        sprintf(json_with_quote, "'%s'", data);

        char station_id_with_quote[strlen(id) + 5];
        sprintf(station_id_with_quote, "'%s'", id);

        char formatted_timestamp[strlen("(SELECT TIMESTAMPTZ 'epoch' + ") + strlen(" * INTERVAL '1 second')") + strlen(timestamp) + 5];
        sprintf(formatted_timestamp, "(SELECT TIMESTAMPTZ 'epoch' + %s * INTERVAL '1 second')", timestamp);

        char* cols[] = {"time", "station_code", "payload"};
        char* vals[] = {
            formatted_timestamp,
            station_id_with_quote,
            json_with_quote
        };
        int n = sizeof(cols)/sizeof(*cols);

        int status = pgsql_exec_insert("data", cols, vals, n);

        if (status != PGSQL_STATUS_OK)
            printf("Insertion failed!\n\n");
        else
            printf("Data inserted!\n\n");

        if (payload != NULL)
            free(payload);

            
        printf("QUEUE COUNT: %d\n\n", ++qcnt);
    }

    printf("B");
    message_processing = 0;
    printf("A");
}

int message_arrived_custom_handler(void *context, char *topicName, int topicLen, MQTTClient_message* message)
{
    char* payload = (char*)malloc(sizeof(char)*(message->payloadlen+1));
    strncpy(payload, (char*)message->payload, message->payloadlen);
    *(payload + message->payloadlen) = '\0';
    enqueue(&queue, payload);
    printf("Message added to queue. Queue size: %d\n", queue.size); fflush(stdout);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    if (!message_processing)
    {
        message_processing = 1;
        printf("============= NEW THREAD! ============\n");
        pthread_create(&process_message_tid, NULL, process_message, NULL);
    }

    printf("MESSAGE: %d\n", ++cnt);

    return 1;
}


int main(int argc, char* argv[])
{
    queue_init(&queue);

    printf("Opening database connection...\n");
    pgsql_open_connection(CONN_INFO);

    mqtt_set_message_arrived_handler(message_arrived_custom_handler);

    printf("Opening sub MQTT connection...\n");
    sub_client = mqtt_open_connection(ADDRESS, CLIENT_SID, MQTT_ASYNC);

    printf("Subscribing to topic %s\n", TOPIC_SUB);
    mqtt_subscribe(sub_client, TOPIC_SUB, QOS);

    while (1) pause();

    return 0;
}