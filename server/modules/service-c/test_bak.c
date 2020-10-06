#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <queue.h>
#include <mqtt.h>
#include <postgresql.h>

#define ADDRESS     "tcp://datalogger.ddns.net:1883"   
#define CLIENT_PID  "ClientPub"
#define CLIENT_SID  "ClientSub"
#define TOPIC_PUB   "response"
#define TOPIC_SUB   "data/#"
#define PAYLOAD     "123456789:id:{\"info1\":\"data1\",\"info2\":\"data2\"}"
#define QOS         1
#define TIMEOUT     10000L

MQTTClient* pub_client;
MQTTClient* sub_client;

Queue queue;
FILE* fp;

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

int main(int argc, char* argv[])
{
    queue_init(&queue);
    fp = fopen("record", "w");
    if (fp == NULL)
    {
        printf("Can't open file for writing\n");
        return 1;
    }

    mqtt_set_message_arrived_handler(message_arrived_custom_handler);
    mqtt_set_message_delivered_handler(message_delivered_custom_handler);

    printf("Opening pub MQTT connection...\n");
    pub_client = mqtt_open_connection(ADDRESS, CLIENT_PID);

    printf("Opening sub MQTT connection...\n");
    sub_client = mqtt_open_connection(ADDRESS, CLIENT_SID);

    printf("Subscribing to topic %s\n", TOPIC_SUB);
    mqtt_subscribe(sub_client, TOPIC_SUB, QOS);

    while (1)
    {
        printf("Press enter to send message\n"); getchar();

        mqtt_publish(pub_client, TOPIC_PUB, PAYLOAD, QOS, TIMEOUT);
    }

    return 0;
}
