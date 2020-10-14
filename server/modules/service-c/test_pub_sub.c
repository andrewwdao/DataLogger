#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <queue.h>
#include <mqtt.h>
#include <pgsql.h>

// PGSQL params
#define CONN_INFO "dbname=test host=datalogger.ddns.net user=postgres password=admin123"

// MQTT params
#define ADDRESS     "tcp://datalogger.ddns.net:1883"   
#define CLIENT_PID  "ClientPub"
#define CLIENT_SID  "ClientSub"
#define TOPIC_PUB   "data"
#define TOPIC_SUB   "data/#"
#define PAYLOAD     "123456789:id:{\"info1\":\"data1\",\"info2\":\"data2\"}"
#define QOS         1
#define TIMEOUT     10000L

MQTTClient* pub_client;
MQTTClient* sub_client;

Queue queue;
FILE* fp;

int message_arrived_test(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    if (*(char*)message->payload == 'q')
    {
        fclose(fp);
        exit(0);
    }
    
    fprintf(fp, "%s\n", (char*)message->payload);
    printf("Message received %s\n", (char*)message->payload);
}

int message_arrived_custom_handler(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char* payload = (char*)message->payload;
    char* timestamp = payload;
    char* id = strstr(payload, ":"); *(id++) = '\0';
    char* data = strstr(id, ":"); *(data++) = '\0';
    *(payload + message->payloadlen) = '\0';

    printf("===============================================\n");
    printf("    Timestamp: %s\n", timestamp);
    printf("    ID: %s\n", id);
    printf("    Data: %s\n", data);
    printf("===============================================\n");

    // printf("message_arrived_custom_handler \n");
    // printf("     topic: %s \n", topicName);
    // printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    // printf("   message: %s\n", (char*)message->payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void message_delivered_custom_handler(void *context, MQTTClient_deliveryToken dt)
{
    printf("message_delivered_custom_handler\n");
    // deliveredtoken = dt;
}

int sql_backup(int argc, char* argv[])
{
    pgsql_open_connection(CONN_INFO);

    char* cols[] = {"col1", "col2", "col3"};
    char* vals[] = {"val1", "val2", "val3"};
    int n = sizeof(cols)/sizeof(*cols);
    pgsql_exec_insert("data", cols, vals, n);

    return 0;
}


int main(int argc, char* argv[])
{
    // ./test client_id <start> <stop>
    if (argc < 2)
    {
        printf("Must provide client id\n.");
        return 1;
    }

    if (argc < 3)
    {
        fp = fopen("record", "w");
        if (fp == NULL)
        {
            printf("Can't open file for writing\n");
            return 1;
        }

        mqtt_set_message_arrived_handler(message_arrived_test);

        printf("Opening sub MQTT connection...\n");
        sub_client = mqtt_open_connection(ADDRESS, argv[1]);

        printf("Subscribing to topic %s\n", TOPIC_SUB);
        mqtt_subscribe(sub_client, TOPIC_SUB, QOS);

        while (1);
    }
    else
    {
        time_t now;
        time(&now);
        now += atoi(argv[2]);

        pub_client = mqtt_open_connection(ADDRESS, argv[1]);
        mqtt_set_message_delivered_handler(message_delivered_custom_handler);

        for (int i = atoi(argv[2]); i <= atoi(argv[3]); i++)
        {
            char payload[200];
            snprintf(payload, 200, "%d:%s:%s", (unsigned)now++, "a", "{\"al\":3.2,\"p01\":4.7}");
            mqtt_publish(pub_client, TOPIC_PUB, payload, QOS, TIMEOUT);
        }
    }
    

    // mqtt_set_message_arrived_handler(message_arrived_test);
    // mqtt_set_message_delivered_handler(message_delivered_custom_handler);

    // printf("Opening pub MQTT connection...\n");
    // pub_client = mqtt_open_connection(ADDRESS, CLIENT_PID);

    // printf("Opening sub MQTT connection...\n");
    // sub_client = mqtt_open_connection(ADDRESS, CLIENT_SID);

    // printf("Subscribing to topic %s\n", TOPIC_SUB);
    // mqtt_subscribe(sub_client, TOPIC_SUB, QOS);

    // while (1)
    // {
    //     printf("Press enter to send message\n"); getchar();

    //     mqtt_publish(pub_client, TOPIC_PUB, PAYLOAD, QOS, TIMEOUT);
    // }
}