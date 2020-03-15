#include <PushButton.h>

#define NUMBER_OF_BUTTONS 10

int buttonPins[NUMBER_OF_BUTTONS] = {
  22, 23, 24, 25, 26,
  27, 28, 29, 30, 31,
};

PushButton buttons[NUMBER_OF_BUTTONS] = {
  (22), (23), (24), (25), (26),
  (27), (28), (29), (30), (31),
};


void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    PushButton* button = &buttons[i];

    button->setActiveLogic(LOW);
    button->setHoldTime(1000);
  }
}

void loop() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    int pin = buttonPins[i];
    PushButton* button = &buttons[i];

    button->update();

    if (button->isClicked()){
      Serial.print("clicked ");
      Serial.println(pin);
    }
        if (button->isDoubleClicked()){
      Serial.print("double clicked ");
      Serial.println(pin);
    }
        if (button->isHeld()){
      Serial.print("held ");
      Serial.println(pin);
    }
//    if (button->isActive()){
//      Serial.println(");
//    }

  }
}
