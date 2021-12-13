#include <ESP8266WiFi.h>
#include "StringSplitter.h"
#include <SoftwareSerial.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>

#define WIFI_SSID "rahayu_plus"
#define WIFI_PASSWORD "Desember1ok"
#define MQTT_HOST "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_PUB_TEMP "wqms/temperature"
#define MQTT_PUB_PH "wqms/ph"
#define MQTT_PUB_TBD "wqms/turbidity"

//Rx=D2 & Tx=D1
SoftwareSerial espSerial(D2,D1);
String incomingByte;
String buff[4];

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;
const long interval = 10000;

///////////////////////////////////////////////////////////////

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach();
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
//  Serial.print("Session present: ");
//  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged. \n");
//  Serial.print("  packetId: ");
//  Serial.println(packetId);
}

///////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  espSerial.begin(4800);

  pinMode(D1, INPUT);
  pinMode(D2, OUTPUT);

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  
  connectToWifi();
}

///////////////////////////////////////////////////////////////

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if(espSerial.available() > 0){
      incomingByte=espSerial.readStringUntil('\n');
      StringSplitter *splitter = new StringSplitter(incomingByte, ',', 3);
      int itemCount = splitter->getItemCount();

      for(int i = 0; i < itemCount; i++){
        buff[i] = splitter->getItemAtIndex(i);
      }

      if(buff[0].toInt()>5){
        uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(buff[0]).c_str());
      }
      if(buff[1].toInt()>5){
        uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_PH, 1, true, String(buff[1]).c_str());
      }
      if(buff[2].toInt()>5){
        uint16_t packetIdPub3 = mqttClient.publish(MQTT_PUB_TBD, 1, true, String(buff[2]).c_str());
      }
      
      Serial.print(F("Temp:"));Serial.print(buff[0]);Serial.print(F(" pH:"));Serial.print(buff[1]);Serial.print(F("TBD:"));Serial.println(buff[2]);    
    }
  }                        
}
