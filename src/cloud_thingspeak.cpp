
/**
 * @file cloud_thingspeak.cpp
 * @author Silvia Martinez @isotopaMaker
 * @date January 2022 
 * @brief Particle meter sensor on ESP32 with bluetooth GATT notify server
 * @license GPL3
 */


#include <cloud_thingspeak.hpp>
#include <wifi.hpp>
#include "ThingSpeak.h"

unsigned long myChannelNumber = 000000; //thingspeak channel ID
const char * myWriteAPIKey = "xxxxxxxxxxxxxxxx";

char thingSpeakAddress[] = "api.thingspeak.com"; //thingtweet API
String thingtweetAPIKey = "xxxxxxxxxxxxxxxx";

int number = 0;

/******************************************************************************
*  T H I N G S P E A K   M Q T T   M E T H O D S
******************************************************************************/

WiFiClient netthingspeak;
MQTTClient cliente(MQTT_BUFFER_SIZE);

void thingspeakPublish() {
    static uint_fast64_t mqttTimeStamp = 0;
    if (millis() - mqttTimeStamp > cfg.stime * 1000 * 2) {
        mqttTimeStamp = millis();

      float pm25 = sensors.getPM25();  // float humi = sensors.getHumidity();
      // if (pm25 == 0.0) humi = sensors.getCO2humi();    // if (humi == 0.0) humi = sensors.getCO2humi();
      // float temp = sensors.getTemperature();      // float temp = sensors.getTemperature();
      // if (temp == 0.0) temp = sensors.getCO2temp();     // if (temp == 0.0) temp = sensors.getCO2temp();

      //  StaticJsonDocument<MQTT_BUFFER_SIZE> doc;
     //   char buffer[MQTT_BUFFER_SIZE];

      //  doc["id"] = cfg.getStationName();
      //  doc["CO2"] = String(sensors.getCO2());
      //  doc["humidity"] = String(humi);
       // doc["temperature"] = String(temp);
      //  doc["pressure"] = String(sensors.getPressure());
       // doc["altitude"] = String(sensors.getAltitude());
       // doc["gas"] = String(sensors.getGas());
       // doc["pm1"] = sensors.getPM1();
       // doc["pm25"] = sensors.getPM25();
       // doc["pm10"] = sensors.getPM10();
       // doc["geo"] = cfg.geo;
      //  doc["battery"] = String(gui.getBatteryLevel());
      //  doc["VBat"] = String(gui.getBatteryVoltage());

       // size_t n = serializeJson(doc, buffer);

       // if (client.publish(thingspeak_TOPIC, buffer, n)) {
       //     if (cfg.devmode) Serial.printf("-->[MQTT] thingspeak sensor payload published. (size: %d)\n", n);
      //  } else {
      //      Serial.printf("[E][MQTT] thingspeak publish sensor data error: %d\n",client.lastError());
      //  }

      // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
     // pieces of information in a channel.  Here, we write to field 1.
        int x = ThingSpeak.writeField(myChannelNumber, 1, pm25, myWriteAPIKey);
         if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the value
  number++;
  if(number > 99){
    number = 0;
  }
  
  delay(20000); // Wait 20 seconds to update the channel again

    }
}

static uint_fast64_t mqttDelayedStamp = 0;

void thingspeakConnect() {
    if (!(cfg.isWifiEnable() && WiFi.isConnected())) return;

    if (millis() - mqttDelayedStamp > MQTT_DELAYED_TIME * 1000) {
        Serial.printf("-->[MQTT] connecting to %s..", thingspeak_HOST);
        int mqtt_try = 0;
        while (mqtt_try++ < MQTT_RETRY_CONNECTION && !cliente.connect(cfg.getStationName().c_str())) {
            Serial.print(".");
            delay(100);
        }
        if (mqtt_try >= MQTT_RETRY_CONNECTION && !cliente.connected()) {
            mqttDelayedStamp = millis();
            Serial.println("\tconnection failed!");
            return;
        }
        mqttDelayedStamp = millis();
        Serial.println("\tconnected!");
        cliente.subscribe(thingspeak_TOPIC);
    }
}

bool thingspeakIsConnected() {
    return cliente.connected();
}

void thingspeakInit() { 
    //cliente.begin(ANAIRE_HOST, ANAIRE_PORT, netAnaire);
     WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(netthingspeak); 
    mqttDelayedStamp = millis() - MQTT_DELAYED_TIME * 1000;
    thingspeakConnect();
}

void thingspeakLoop () {
    if(!WiFi.isConnected()) return; 
    cliente.loop();
    delay(10);
    if (!cliente.connected()) thingspeakConnect();
    thingspeakPublish();
}