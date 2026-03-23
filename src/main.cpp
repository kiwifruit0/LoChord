#include "Config/config.h"
#include <Arduino.h>
#include <Button.h>

Button buttons[NUM_BUTTONS];

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].begin(buttonPins[i]);
  }
}

void loop() {
  // logic
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();

    if (buttons[i].wasPressed()) {
      Serial.println("Button " + String(i) + " was pressed!");
    } else if (buttons[i].wasReleased()) {
      Serial.println("Button " + String(i) + " was released!");
    }
  }
}
