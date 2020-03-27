// using library https://github.com/avandalen/avdweb_Switch
#include <avdweb_Switch.h>

#include <WiFiLink.h>
#include <PubSubClient.h>  

#include "secrets.h"

// connection to ESP8266, for wifi
#if !defined(ESP_CH_SPI) && !defined(HAVE_HWSERIAL3)
  #include "SoftwareSerial.h"
  SoftwareSerial Serial3(15, 14); // RX, TX
#endif

char* MQTT_CLIENT_NAME = "downstairs/1/"; // location/devicenumber
char* MQTT_BUTTON_TOPIC = "button/"; // location/devicenumber/inputtype/

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void(* resetFunc) (void) = 0; // declare reset function @ address 0

int buttonPins[] = {
  A0, 1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, 
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 
  // 14, 15, needed for Serial3 connection to ESP8266, for wifi
  16, 17, 18, 19, 20, 21, 
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
};

#define NUMBER_OF_BUTTONS sizeof(buttonPins)/sizeof(int)

Switch *buttons[NUMBER_OF_BUTTONS];

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Setting up wifi");
  
  setupWifi();
  setupMqtt();
  setupButtons();
}

void loop() {  
  ensureMqttConnectionActive();

  mqttClient.loop();
  
  processButtons();
}

void setupButtons(){
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    buttons[i] = new Switch(buttonPins[i]);
  }

  Serial.print("Initialized ");
  Serial.print(NUMBER_OF_BUTTONS);
  Serial.println(" buttons");
}

void processButtons(){
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    int pin = buttonPins[i];
    Switch* button = buttons[i];

    processButton(button, pin);
  }
}

void processButton(Switch* button, int pin) {
  button->poll();

  if (button->singleClick()){
    publishButton(pin, "single");
  }
  
  if (button->doubleClick()){
    publishButton(pin, "double");
  }
      
  if (button->longPress()){
    publishButton(pin, "hold");
  }
}

void publishButton(int pin, char* state){
    char topicBuffer[30];
    sprintf(topicBuffer, "cmnd/%s%s%d", MQTT_BUTTON_TOPIC, MQTT_CLIENT_NAME, pin);

    Serial.print("Sending MQTT message '");
    Serial.print(state);
    Serial.print("' to topic ");
    Serial.println(topicBuffer);
    
    mqttClient.publish(topicBuffer, state);
}

void setupWifi() {  
  #if !defined(ESP_CH_SPI)
    Serial3.begin(9600); // speed must match with BAUDRATE_COMMUNICATION setting in firmware config.h
    WiFi.init(&Serial3);
  #endif
  
  if (WiFi.checkFirmwareVersion("1.1.0")) {
    Serial.println("resetting ESP");
    WiFi.resetESP(); // to clear 'sockets' after sketch upload
    delay(5000); // wait while WiFiLink firmware connects to WiFi
  }
  
  if (WiFi.status() == WL_NO_WIFI_MODULE_COMM) {
    delay(2000);
    Serial.println("communication with WiFi module not established. Resetting...");
    resetFunc(); 
  }
  
  do {
    Serial.println("attempting to connect to wifi");

    delay(1000); // wait for connection
  }
  while ( WiFi.status() != WL_CONNECTED);

  Serial.println("connected to wifi");
}

void setupMqtt(){
  mqttClient.setServer(_MQTT_BROKER, _MQTT_PORT);

  Serial.println("initialized MQTT client");
}

void ensureMqttConnectionActive(){
  while (!mqttClient.connected()){
    Serial.println("connecting to MQTT");

    if(!mqttClient.connect(MQTT_CLIENT_NAME)){
      Serial.print("failed, rc = ");
      Serial.print(mqttClient.state());
      Serial.println(", trying again in 5 seconds");

      delay(5000);

      continue;
    }
    
    Serial.println("connection established");
  }
}
