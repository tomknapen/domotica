#include <PushButton.h>

#define NUMBER_OF_BUTTONS 10

PushButton buttons[NUMBER_OF_BUTTONS] = {
  (22), (23), (24), (25), (26),
  (27), (28), (29), (30), (31),
};


void setup() {
  Serial.begin(115200);

  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);
  pinMode(31, INPUT_PULLUP);
  
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    PushButton* button = &buttons[i];

    button->setActiveLogic(LOW);
    button->setHoldTime(1000);
  }
}

void loop() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    PushButton* button = &buttons[i];

    button->update();

    if (button->isClicked()){
      Serial.println("clicked");
    }
        if (button->isDoubleClicked()){
      Serial.println("double clicked");
    }
        if (button->isHeld()){
      Serial.println("held");
    }
    if (button->isActive()){
//      Serial.println("active");
    }
  }
}
