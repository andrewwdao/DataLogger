#include <MQTTClient.h>

int mqtt_open_connection(const char*,const char*);
int mqtt_disconnect();

int mqtt_subscribe(const char*,int);
int mqtt_unsubscribe(const char*);

int mqtt_publish(char*,char*,int,int);
