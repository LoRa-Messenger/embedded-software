#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <LoRaMessage.h>

#define BAUDRATE 9600

void setup() {
  // put your setup code here, to run once:

  Serial.begin(BAUDRATE);

  // test if the private LoRaMessage library works
  RegularMessage message(1, 0, 0, 10000000, "hello, world");
  Serial.println("hello, world");
}

void loop() {
  // put your main code here, to run repeatedly:
}