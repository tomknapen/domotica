#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include "secrets.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int relays[] = { D1, D2, D3, D4, D5, D6, D7 };

#define NUMBER_OF_RELAYS sizeof(relays)/sizeof(int)

char* MQTT_CLIENT_NAME = "downstairs/"; // location/devicenumber
char* MQTT_RELAY_TOPIC = "relay/"; // location/devicenumber/inputtype/

void setup() {  
  Serial.begin(115200);

  setupRelays();

  setupWifi();
  setupMqtt();
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
  if (strcmp(strtok(topic, "/"), "cmnd") != 0){
    return;
  }
  
  if (strcmp(strtok(NULL, "/"), "relay") != 0){
    return;
  }
  
  if (strcmp(strtok(NULL, "/"), "downstairs") != 0){
    return;
  }

  int relayNumber = atoi(strtok(NULL, "/"));

  if (relayNumber < 1 || relayNumber > NUMBER_OF_RELAYS){
    return;
  }

  int relay = relays[relayNumber - 1]; // number in kast is 1-based
  
  char* state = getPayloadMsg(payload, length);

  Serial.print("Received message for relay ");
  Serial.print(relayNumber);
  Serial.print(" = pin ");
  Serial.print(relay);
  Serial.print(": ");
  Serial.println(state);

  processRelay(relay, relayNumber, state);
  
}

void processRelay(int relay, int relayNumber, char* state){
  if(strcmp("OFF", state) == 0){
    digitalWrite(relay, LOW);
    publishRelay(relayNumber, "OFF");
  }  
  if(strcmp("ON", state) == 0){
    digitalWrite(relay, HIGH);
    publishRelay(relayNumber, "ON");
  }
}

void publishRelay(int relay, char* state){
  char topicBuffer[30];
  sprintf(topicBuffer, "stat/%s%s%d", MQTT_RELAY_TOPIC, MQTT_CLIENT_NAME, relay);

  Serial.print("Sending MQTT message '");
  Serial.print(state);
  Serial.print("' to topic ");
  Serial.println(topicBuffer);
  
  mqttClient.publish(topicBuffer, state);
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

    mqttClient.subscribe("cmnd/relay/downstairs/#");
  }
}
