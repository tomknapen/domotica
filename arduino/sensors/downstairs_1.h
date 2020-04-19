char* MQTT_CLIENT_NAME = "downstairs/1/";
long WIFI_BAUD = 115200;

int ledPins[] = {
  
};

int buttonPins[] = {
  //  A0, 1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, // probably messes up some mqtt communication when pressed :/
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 
  // 14, 15, // needed for Serial3 connection to ESP8266, for wifi
  16, 17, 18, 19, 20, 21, 
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
};
