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
// #define ADDRESS     "tcp://datalogger.ddns.net:1883"
#define ADDRESS     "datalogger.ddns.net"
#define SUB_CMD     "mosquitto_sub -h " ADDRESS " -t data"
#define CLIENT_SID  "ClientSub"
#define TOPIC_SUB   "data"
#define QOS         2
#define TIMEOUT     1000

#define MESSAGE_HANDLED_OK  0
#define MESSAGE_HANDLED_ERR 1


MQTTClient* sub_client;

pthread_t process_message_tid;
volatile int message_processing;
volatile int cnt = 0;

Queue queue;


int handle_message(char* payload)
{
    char* timestamp = payload;
    char* id = strstr(payload, ":");
    if (id == NULL)
    {
        printf("Payload format is not correct!\n%s\nAborted!\n", payload);
        free(payload);
        return MESSAGE_HANDLED_ERR;
    }
    else *(id++) = '\0';

    char* data = strstr(id, ":");
    if (id == NULL)
    {
        printf("Payload format is not correct!\n%s\nAborted!\n", payload);
        free(payload);
        return MESSAGE_HANDLED_ERR;
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

    if (status != PGSQL_STATUS_OK) {
        printf("Insertion failed!\n\n");

        *(--id) = ':';
        *(--data) = ':';
        enqueue(&queue, payload);

        return MESSAGE_HANDLED_ERR;
    }
    
    printf("Data inserted!\n\n");

    if (payload != NULL) free(payload);

    return MESSAGE_HANDLED_OK;
}


int main(int argc, char* argv[])
{
    queue_init(&queue);

    printf("Opening database connection...\n");
    pgsql_open_connection(CONN_INFO);

    printf("Opening sub MQTT connection...\n");
    FILE* sub_client = popen(SUB_CMD, "r");

    char message[300];
    while(fgets(message, sizeof(message), sub_client) != NULL) {
        char* m_message = (char*)malloc(sizeof(char)*strlen(message)+1);
        strcpy(m_message, message);
        enqueue(&queue, m_message);

        while (queue.size) {
            char* payload = dequeue(&queue);
            if (handle_message(payload) != MESSAGE_HANDLED_OK) break;
        }

        printf("COUNT: %d\n\n", ++cnt);
    }


    return 0;
}