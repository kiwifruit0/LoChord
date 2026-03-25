#include "USB.h"
#include "USBMIDI.h"
#include <Arduino.h>

USBMIDI MIDIDevice;

void setup() {
  MIDIDevice.begin();
  USB.begin();
  delay(2000);
}

void loop() {
  MIDIDevice.noteOn(60, 64, 0);
  delay(500);
  MIDIDevice.noteOff(60, 0, 0);
  delay(500);
}
