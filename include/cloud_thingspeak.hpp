#define thingspeak_HOST "mqtt.thingspeak.com"
#define thingspeak_TOPIC "measurement"
#define thingspeak_PORT 80

void thingspeakLoop ();
void thingspeakInit ();
bool thingspeakIsConnected();