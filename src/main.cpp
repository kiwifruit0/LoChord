#include "USB.h"
#include "USBCDC.h"
#include <Arduino.h>

USBCDC USBSerial;

void setup() {
  USB.begin();
  USBSerial.begin(115200);

  delay(2000);

  // Wait until host is ready (important)
  while (!USBSerial) {
    delay(10);
  }

  USBSerial.println("USB CDC DIRECT");
}

void loop() {
  USBSerial.println("HELLO");
  delay(1000);
}
