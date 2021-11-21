#include <Arduino.h>
#include <LoRaMessage.h>

void setup() {
  // put your setup code here, to run once:


  // test if the private LoRaMessage library works
  RegularMessage message(1, 0, 0, 10000000, "hello, world");
}

void loop() {
  // put your main code here, to run repeatedly:
}