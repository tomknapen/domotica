/**
  Arduino MEGA 2560 MQTT I/O controller.
  Copyright 2018 Peter Illmayer peter@illmayer.com
  Acknowledge SuperHouse Automation Pty Ltd <info@superhouse.tv>
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  2019 Jeroen Schaeken
  Use of WiFiLink to connect to wifi using an ESP8266
*/

// Libraries
#include <stdio.h>
#include <string.h>
#include <SPI.h>                                   // For networking
//#include <Ethernet.h>                              // For networking
#include <WiFiLink.h>
#include <PubSubClient.h>                          // For MQTT
#include "Wire.h"                                  // For MAC address ROM
#include <Adafruit_SleepyDog.h>

#if !defined(ESP_CH_SPI) && !defined(HAVE_HWSERIAL3)
#include "SoftwareSerial.h"
SoftwareSerial Serial3(15, 14); // RX, TX
#endif

char ssid[] = "";     //  your network SSID (name)
char pass[] = "";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status


// Defines
#define MAC_I2C_ADDRESS                 0x50       // Microchip 24AA125E48 I2C ROM address
#define WATCHDOG_PIN                    3          // Output to pat the watchdog
#define WATCHDOG_PULSE_LENGTH           50         // Watchdog timer pulse length in Milliseconds
#define WATCHDOG_RESET_INTERVAL         30000      // Watchdog timer timer in Milliseconds. Also the period for sensor reports.
#define ENABLE_DHCP                     true       // true/false
#define ENABLE_MAC_ADDRESS_ROM          false      // true/false
#define ENABLE_EXTERNAL_WATCHDOG        false      // true / false
#define DEBOUNCE_DELAY 50

/*--------------------------- Configuration ------------------------------*/
IPAddress broker(192, 168, 1, 200);                                  // MQTT broker
int mqttPort = 1883;                                                // MQTT port
IPAddress ip(192, 168, 1, 35);                                      // Default if DHCP is not used
const char* user  = "123";                                             // MQTT user
const char* password  = "456";                                         // MQTT password
byte buttonId;
byte lastButtonPressed = 0;
byte bufferIndex;
char tempStr[10] = "";
char tempStr1[10] = "";
char messageBuffer[100];                                             // MQTT Message buffer size
char topicBuffer[100];                                               // MQTT Topic buffer size
char clientBuffer[50];                                               // MQTT Temporary buffer
char outputBuffer[50];                                               // MQTT Output subscribe topic buffer size
char willTopic[50];
char* myPin = "";
const char* statusTopic  = "tele/megga";                                 // MQTT topic to publish status reports
int InitialsensorReading = 0;
int panelId = 20;
int tempInt;
int OutputNumber, myPinIndex, myOutputPin;
long lastActivityTime   = 0;
long watchdogLastResetTime = 0;
String messageString = "";
String topicString = "";
//String tempStr1="";
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };       // Set if no MAC ROM
unsigned long timeLater = 0;

byte willQoS = 0;
const char* willMessage = "Disconnected";
boolean willRetain = true;
boolean retained = true;

//EthernetClient ethclient;
WiFiClient ethclient;
PubSubClient client(ethclient);

/* ************************************************************************************* */
/* Button setup */
static byte lastButtonState[24] = {   0,  0,  0,  0,    0,  0,  0,  0,
                                      0,  0,  0,  0,    0,  0,  0,  0,
                                      0,  0,  0,  0,    0,  0,  0,  0
                                  };

static byte buttonArray[24]     = {  54, 55, 56, 57,   58, 59, 60, 61,     // A0-A7
                                     62, 63, 64, 65,   66, 67, 68, 69,     // A8-A15
                                     40, 41, 42, 43,   44, 45, 46, 47
                                  };   // D40-D47


static byte outputArray[24]      = { 16, 17, 18, 19,   20, 21, 22, 23,     // D16-D23
                                     24, 25, 26, 27,   28, 29, 30, 31,     // D24-D31
                                     32, 33, 34, 35,   36, 37, 38, 39
                                   };   // D32-D39


void(* resetFunc) (void) = 0; //declare reset function @ address 0
void setup()
{
  Serial.begin(115200);  // Use the serial port to report back readings
  /* Set up the watchdog timer */
  if (ENABLE_EXTERNAL_WATCHDOG == true)
  {
    pinMode(WATCHDOG_PIN, OUTPUT);
    digitalWrite(WATCHDOG_PIN, LOW);
  }

  Wire.begin();        // Wake up I2C bus

  if ( ENABLE_MAC_ADDRESS_ROM == true )
  {
    Serial.print(F("Getting MAC address from ROM: "));
    mac[0] = readRegister(0xFA);
    mac[1] = readRegister(0xFB);
    mac[2] = readRegister(0xFC);
    mac[3] = readRegister(0xFD);
    mac[4] = readRegister(0xFE);
    mac[5] = readRegister(0xFF);
  } else {
    Serial.print(F("Using static MAC address: "));
  }
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


#if !defined(ESP_CH_SPI)
  Serial3.begin(115200); // speed must match with BAUDRATE_COMMUNICATION setting in firmware config.h
  WiFi.init(&Serial3);
#endif
  if (WiFi.checkFirmwareVersion("1.1.0")) {
    WiFi.resetESP(); // to clear 'sockets' after sketch upload
    delay(5000); // let firmware initialize
  }

  //Check if communication with the wifi module has been established
  while (WiFi.status() == WL_NO_WIFI_MODULE_COMM) {
    delay(2000);
    Serial.println("reset");
    resetFunc();  //call reset
  }

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  Serial.println("Setting input pull-ups");
  for ( byte i = 0; i < 24; i++)
  {
    pinMode(buttonArray[i], INPUT_PULLUP);
    Serial.print(buttonArray[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.println("Setting Outputs");
  for ( byte i = 0; i < 24; i++)
  {
    pinMode(outputArray[i], OUTPUT);
    digitalWrite(outputArray[i], HIGH);
    Serial.print(outputArray[i]);
    Serial.print(" ");
  }
  Serial.println();

  /* Connect to MQTT broker */
  Serial.println("connecting...");
  client.setServer(broker, mqttPort);
  client.setCallback(callback);
  String clientString = "Starting Arduino-" + WiFi.localIP();
  clientString.toCharArray(clientBuffer, clientString.length() + 1);
  client.publish(statusTopic, clientBuffer);

  Serial.println("Ready.");

  // Read all of the Inputs and send to the MQTT broker on the STAT topic
  for ( buttonId = 0; buttonId < 24; buttonId++) {
    InitialsensorReading = digitalRead( buttonArray[buttonId] );
    lastButtonState[buttonId] = InitialsensorReading;

    // Send input data to broker
    // on prescribed topic and payload

    messageString = String(InitialsensorReading);
    messageString.toCharArray(messageBuffer, messageString.length() + 1);
    topicString = "stat/" + String(panelId) + "/Input/" + String(buttonId + 1);
    topicString.toCharArray(topicBuffer, topicString.length() + 1);

    if (!client.connected()) {
      reconnect();
    }
    client.publish(topicBuffer, messageBuffer);
  }
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  runHeartbeat();

  client.loop();

  byte i;
  for ( i = 0; i < 24; i++) {
    processButtonDigital( i );
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // String clientString = "Reconnecting Arduino-" + String(Ethernet.localIP());

    sprintf(willTopic, "tele/%d", panelId);


    String clientString = "Connected";
    clientString.toCharArray(clientBuffer, clientString.length() + 1);
    if (client.connect(panelId, willTopic, willQoS, willRetain, willMessage)) {
      //  if (client.connect(clientBuffer)) {
      Serial.println("connected to broker");
      // Once connected, publish an announcement...
      clientString.toCharArray(clientBuffer, clientString.length() + 1);
      client.publish(willTopic, clientBuffer);
      // ... and resubscribe
      sprintf(outputBuffer, "cmnd/%d/#", panelId);
      client.subscribe(outputBuffer);

      for ( buttonId = 0; buttonId < 24; buttonId++) {
        InitialsensorReading = digitalRead( buttonArray[buttonId] );
        lastButtonState[buttonId] = InitialsensorReading;

        // Send input data to broker
        // on prescribed topic and payload

        messageString = String(InitialsensorReading);
        messageString.toCharArray(messageBuffer, messageString.length() + 1);
        topicString = "stat/" + String(panelId) + "/Input/" + String(buttonId + 1);
        topicString.toCharArray(topicBuffer, topicString.length() + 1);
        client.publish(topicBuffer, messageBuffer);
      }

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void runHeartbeat()
{
  if ((millis() - watchdogLastResetTime) > WATCHDOG_RESET_INTERVAL) // Is it time to run yet?
  {
    /*
      String topicString = "tele/" + String(panelId) + "/Status";
      topicString.toCharArray(topicBuffer, topicString.length()+1);
      String messageString = "Alive";
      messageString.toCharArray(messageBuffer, messageString.length()+1);
      if (client.publish(topicBuffer, messageBuffer))
    */
    {
      patWatchdog();  // Only pat the watchdog if we successfully published to MQTT
    }
  }
}

void processButtonDigital( byte buttonId )
{
  int sensorReading = digitalRead( buttonArray[buttonId] );

  if ( sensorReading == 0 ) // Input pulled low to GND. Button pressed.
  {
    if (lastButtonState[buttonId] == 0)  // The button was previously un-pressed
    {
      if ((millis() - lastActivityTime) > DEBOUNCE_DELAY) // Proceed if we haven't seen a recent event on this button
      {
        lastActivityTime = millis();
        lastButtonPressed = buttonId;

        String messageString = "0";
        messageString.toCharArray(messageBuffer, messageString.length() + 1);

        String topicString = "stat/" + String(panelId) + "/Input/" + String(buttonId + 1);
        topicString.toCharArray(topicBuffer, topicString.length() + 1);

        client.publish(topicBuffer, messageBuffer);
      }
    }
    lastButtonState[buttonId] = 1;
  }
  else {
    if (lastButtonState[buttonId] == 1)  // The button was previously un-pressed
    {
      if ((millis() - lastActivityTime) > DEBOUNCE_DELAY) // Proceed if we haven't seen a recent event on this button
      {
        lastActivityTime = millis();
        lastButtonPressed = buttonId;

        String messageString = "1";
        messageString.toCharArray(messageBuffer, messageString.length() + 1);

        String topicString = "stat/" + String(panelId) + "/Input/" + String(buttonId + 1);
        topicString.toCharArray(topicBuffer, topicString.length() + 1);

        client.publish(topicBuffer, messageBuffer);
      }
    }
    lastButtonState[buttonId] = 0;
  }
}

void patWatchdog()
{
  if ( ENABLE_EXTERNAL_WATCHDOG )
  {
    digitalWrite(WATCHDOG_PIN, HIGH);
    delay(WATCHDOG_PULSE_LENGTH);
    digitalWrite(WATCHDOG_PIN, LOW);
  }
  watchdogLastResetTime = millis();
}

byte readRegister(byte r)
{
  unsigned char v;
  Wire.beginTransmission(MAC_I2C_ADDRESS);
  Wire.write(r);  // Register to read
  Wire.endTransmission();

  Wire.requestFrom(MAC_I2C_ADDRESS, 1); // Read a byte
  while (!Wire.available())
  {
    // Wait
  }
  v = Wire.read();
  return v;
}

/**
   MQTT callback
*/
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.println();

  }

  sscanf(topic, "%[^/]/%d/%[^/]/%d", tempStr, &tempInt, tempStr1, &OutputNumber);

  Serial.println(tempStr);
  Serial.println(tempInt);
  Serial.println(tempStr1);
  Serial.println(OutputNumber);

  if (strcmp(tempStr1, "Output") == 0) {
    if ((OutputNumber > 0) && (OutputNumber < 25)) {
      myPinIndex = (OutputNumber - 1);
      myOutputPin  = outputArray[myPinIndex];

      if (((char)payload[0] == '1') || ((char)payload[0] == '0')) {
        if ((char)payload[0] == '0') {
          digitalWrite(myOutputPin, HIGH);   // Turn the LED off (Note that LOW is the voltage level
          String text = "Turning output off -> ";
          String serialmessage = text + myOutputPin;
          Serial.println(serialmessage);
        } else {

          digitalWrite(myOutputPin, LOW);  // Turn the LED on by making the voltage HIGH
          String text = "Turning output on -> ";
          String serialmessage = text + myOutputPin;
          Serial.println(serialmessage);
        }
        String topicString = "stat/" + String(panelId) + "/Output/" + (OutputNumber);

        topicString.toCharArray(topicBuffer, topicString.length() + 1);
        String messageString = "0";

        messageString = ((char)payload[0]);
        messageString.toCharArray(messageBuffer, messageString.length() + 1);

        client.publish(topicBuffer, messageBuffer);
      }
    }  else Serial.println("Number not in range for output");

  }
  if (strcmp(tempStr1, "reboot") == 0) {
    Serial.println("We are now going to reboot");
    String clientString = "Rebooting";
    clientString.toCharArray(clientBuffer, clientString.length() + 1);
    client.publish(willTopic, clientBuffer);
    Watchdog.enable(4000);
  }

}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
