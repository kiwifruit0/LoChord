#include "Button.h"
#include "config.h"
#include <Arduino.h>
#include <USB.h>
#include <USBMIDI.h>

USBMIDI MIDIDevice;
Button buttons[NUM_BUTTONS];

void setup() {
  MIDIDevice.begin();
  USB.begin();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].begin(buttonPins[i]);
  }

  delay(2000);
}

void loop() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();

    if (buttons[i].wasPressed()) {
      digitalWrite(LED_BUILTIN, HIGH);
      MIDIDevice.noteOn(60 + i, 64, 0);
    } else if (buttons[i].wasReleased()) {
      digitalWrite(LED_BUILTIN, LOW);
      MIDIDevice.noteOff(60 + i, 0, 0);
    }
  }
}
