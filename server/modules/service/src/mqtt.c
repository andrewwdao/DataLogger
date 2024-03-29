#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MQTTClient.h>


MQTTClient client;
// volatile MQTTClient_deliveryToken deliveredtoken;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

int(*message_delivered_handler)(void*, MQTTClient_deliveryToken) = NULL;
int(* message_arrived_handler)(void*,char*,int,MQTTClient_message) = NULL;

void message_delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    // deliveredtoken = dt;
}

int message_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connection_lost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int mqtt_open_connection(const char* address, const char* client_id)
{
    int rc;

    if ((rc = MQTTClient_create(&client, address, client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        return -1;
    }

    if ((rc = MQTTClient_setCallbacks(client, NULL, connection_lost, message_arrived, message_delivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        return -1;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        return -1;
    }

    return 0;
}

int mqtt_disconnect()
{
    int rc;
    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to disconnect, return code %d\n", rc);
    	rc = EXIT_FAILURE;
        return -1;
    }

    MQTTClient_destroy(&client);
    return 0;
}

int mqtt_subscribe(const char* topic, int qos)
{
    int rc;
    if ((rc = MQTTClient_subscribe(client, topic, qos)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to subscribe, return code %d\n", rc);
    	rc = EXIT_FAILURE;
        return -1;
    }

    return 0;
}

int mqtt_unsubcribe(const char* topic)
{
    int rc;
    if ((rc = MQTTClient_unsubscribe(client, topic)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        return -1;
    }

    return 0;
}

int mqtt_publish(char* topic, char* payload, int qos, int timeout)
{
    MQTTClient_deliveryToken deliveredtoken;
    
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(payload);
    pubmsg.qos = qos;
    pubmsg.retained = 0;

    int rc;
    if ((rc = MQTTClient_publishMessage(client, topic, &pubmsg, &deliveredtoken)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to publish message, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }

    // Wait for publication of message or timeout
    rc = MQTTClient_waitForCompletion(client, deliveredtoken, timeout);
}
