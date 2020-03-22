#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include "secrets.h"

#define ON "ON"
#define OFF "OFF"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int relays[] = { D1, D2, D3, D4, D5, D6, D7 };

#define NUMBER_OF_RELAYS sizeof(relays)/sizeof(int)

void setup() {  
  Serial.begin(115200);

  setupWifi();
  setupMqtt();
  setupRelays();
}

void setupWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt(){
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(onMqttMessage);
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  if (strcmp(strtok(topic, "/"), "downstairs") != 0){
    return;
  }
  
  if (strcmp(strtok(NULL, "/"), "relay") != 0){
    return;
  }

  int relayIndex = atoi(strtok(NULL, "/")) - 1;

  if (relayIndex < 0 || relayIndex >= NUMBER_OF_RELAYS){
    return;
  }

  int relay = relays[relayIndex];
  
  char* state = getPayloadMsg(payload, length);

  Serial.print("Received message for relay ");
  Serial.print(relayIndex + 1);
  Serial.print(" = pin ");
  Serial.print(relay);
  Serial.print(": ");
  Serial.println(state);

  processRelay(relay, state);
}

void processRelay(int relay, char* state){
  if(strcmp(OFF, state) == 0){
    digitalWrite(relay, LOW);
  }  
  if(strcmp(ON, state) == 0){
    digitalWrite(relay, HIGH);
  }
}

char* getPayloadMsg(byte* payload, unsigned int length){
  payload[length] = '\0';
  return (char*) payload;
}

void setupRelays(){
  for (int i = 0; i < NUMBER_OF_RELAYS; i++){
    setupPin(relays[i]);
  }
}

void setupPin(int pin){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
}

void reconnect() {
  while (!mqttClient.connected()){
    Serial.println("connecting to MQTT");

    if(!mqttClient.connect("I am the client")){
      Serial.print("failed, rc = ");
      Serial.println(", trying again in 1 second");

      delay(1000);

      continue;
    }
    
    Serial.println("connection established");

    mqttClient.subscribe("downstairs/relay/#");
  }
}
