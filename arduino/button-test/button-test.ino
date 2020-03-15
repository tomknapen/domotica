// using library https://github.com/avandalen/avdweb_Switch
#include <avdweb_Switch.h>

int buttonPins[] = {
  22, 23, 24, 25, 26,
  27, 28, 29, 30, 31,
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

  if (duration > 2){
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
