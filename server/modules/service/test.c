#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mqtt.h>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientSub"
#define TOPIC       "data/#"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

int main(int argc, char* argv[])
{
    mqtt_open_connection(ADDRESS, CLIENTID);

    // mqtt_subscribe(TOPIC, QOS);

    mqtt_publish("data/haha", "message", QOS, TIMEOUT);

    return 0;
}
