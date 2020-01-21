#define NUMBER_OF_PINS 10
const int buttonPins[NUMBER_OF_PINS] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < NUMBER_OF_PINS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

}

void loop() {
  for (int i = 0; i < NUMBER_OF_PINS; i++) {
    if (digitalRead(buttonPins[i]) == HIGH) {
      continue;
    }

    Serial.print("Input detected on pin ");
    Serial.println(buttonPins[i]);
  }
}
