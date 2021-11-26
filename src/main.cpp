#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <LoRaMessage.h>
#include "heltec.h"

#define BAND 915E6 // LoRa frequency band

void setup() {
  // put your setup code here, to run once:

  // enable OLED display, LoRa, UART serial, and PABOOST; set LoRa frequency band
  // note that this sets serial baudrate to 115200
  Heltec.begin(true, true, true, true, BAND);

  // test if the private LoRaMessage library works
  RegularMessage message(1, 0, 0, 10000000, "hello, world");
}

void loop() {
  // put your main code here, to run repeatedly:
}