#include <MQTTClient.h>

MQTTClient* mqtt_open_connection(const char*,const char*);
int mqtt_disconnect(MQTTClient*);

int mqtt_subscribe(MQTTClient*,const char*,int);
int mqtt_unsubscribe(MQTTClient*,const char*);

int mqtt_publish(MQTTClient*,char*,char*,int,int);
