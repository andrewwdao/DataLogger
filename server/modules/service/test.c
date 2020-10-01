#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mqtt.h>

#define ADDRESS     "tcp://192.168.0.99:1883"   
#define CLIENT_PID  "ExampleClientPub2"
#define CLIENT_SID  "ExampleClientSub"
#define TOPIC_PUB   "data"
#define TOPIC_SUB   "data/#"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

MQTTClient* pub_client;
MQTTClient* sub_client;

int main(int argc, char* argv[])
{
    pub_client = mqtt_open_connection(ADDRESS, CLIENT_PID);
    sub_client = mqtt_open_connection(ADDRESS, CLIENT_SID);

    mqtt_subscribe(sub_client, TOPIC_PUB, QOS);

    while (1)
    {
        printf("Press enter to send message"); getchar();

        mqtt_publish(pub_client, TOPIC_PUB, PAYLOAD, QOS, TIMEOUT);
    }

    return 0;
}
