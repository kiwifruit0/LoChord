#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("light on");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("light off");
}
