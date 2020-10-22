#include <MQTTClient.h>

#define MQTT_ASYNC 1
#define MQTT_SYNC  0

MQTTClient* mqtt_open_connection(const char* address, const char* client_id, int async);
int mqtt_disconnect(MQTTClient* client);

int mqtt_subscribe(MQTTClient* client, const char* topic, int qos);
int mqtt_unsubcribe(MQTTClient* client, const char* topic);

int mqtt_publish(MQTTClient* client, char* topic, char* payload, int qos, int timeout);

void mqtt_set_message_arrived_handler(int(*message_arrived_handler_)(void*,char*,int,MQTTClient_message*));
void mqtt_set_message_delivered_handler(void(*message_delivered_handler_)(void*, MQTTClient_deliveryToken));
