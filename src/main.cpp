#include "USB.h"
#include "USBCDC.h"
#include <Arduino.h>
#include <ButtonCtrl.h>
#include <Control_Surface.h>

USBCDC USBSerial;
USBDebugMIDI_Interface midi{USBSerial};

void setup() {
  USB.begin();
  USBSerial.begin(115200);

  delay(2000);

  while (!USBSerial) {
    delay(10);
  }

  USBSerial.println("USB CDC DIRECT");
}

void loop() {
  USBSerial.println("HELLO");
  delay(1000);
}
