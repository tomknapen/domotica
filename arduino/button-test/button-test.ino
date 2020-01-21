#include <EasyButton.h>

#define NUMBER_OF_BUTTONS 10

EasyButton buttons[NUMBER_OF_BUTTONS] = {
  (22), (23), (24), (25), (26),
  (27), (28), (29), (30), (31),
};

void onButtonPressed() {
  Serial.println("Button pressed!");
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    buttons[i].begin();
    buttons[i].onPressed(onButtonPressed);
  }
}

void loop() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++){
    buttons[i].read();
  }
}
