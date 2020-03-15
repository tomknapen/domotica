// using library https://github.com/avandalen/avdweb_Switch
#include <avdweb_Switch.h>

#define NUMBER_OF_BUTTONS 10

int buttonPins[NUMBER_OF_BUTTONS] = {
  22, 23, 24, 25, 26,
  27, 28, 29, 30, 31,
};

Switch buttons[NUMBER_OF_BUTTONS] = {
  (22), (23), (24), (25), (26),
  (27), (28), (29), (30), (31),
};

void setup() {
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    int pin = buttonPins[i];
    Switch* button = &buttons[i];

    processButton(button, pin);
  }
}

void processButton(Switch* button, int pin) {
  button->poll();

  if (button->singleClick()){
    Serial.print("clicked ");
    Serial.println(pin);
  }
  
  if (button->doubleClick()){
    Serial.print("double clicked ");
    Serial.println(pin);
  }
      
  if (button->longPress()){
    Serial.print("held ");
    Serial.println(pin);
  }
}
