// using library https://github.com/avandalen/avdweb_Switch
#include <avdweb_Switch.h>

int buttonPins[] = {
  A0, 1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, 
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
};

#define NUMBER_OF_BUTTONS sizeof(buttonPins)/sizeof(int)

Switch *buttons[NUMBER_OF_BUTTONS];

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    buttons[i] = new Switch(buttonPins[i]);
  }
}

void loop() {
  int startTime = millis();
  
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    int pin = buttonPins[i];
    Switch* button = buttons[i];

    processButton(button, pin);
  }
  
  int endTime = millis();

  int duration = endTime - startTime;

  if (duration > 3){
    Serial.print("elapsed: ");
    Serial.println(duration);
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
