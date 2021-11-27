#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <LoRaMessage.h>
#include "heltec.h"

#define BAND 915E6 // LoRa frequency band

int counter = 0;

// ID of this device to compare with recipientId for incoming messages
byte deviceId;

// buffer for reading out incoming message headers
byte buf[4];

void setup() {
  // TODO:
  // 1. connect with phone over BLE
  // 2. initialize all the device IDs (so they are all in agreement as to who is who)

  // enable OLED display, LoRa, UART serial, and PABOOST; set LoRa frequency band
  // note that this sets serial baudrate to 115200
  Heltec.begin(true, true, true, true, BAND);
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print("Sending packet: ");
  Serial.println(counter);
  // send packet
  LoRa.beginPacket();
/*
* LoRa.setTxPower(txPower,RFOUT_pin);
* txPower -- 0 ~ 20
* RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
*   - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
*   - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
*/
  LoRa.setTxPower(17, RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();
  
  counter++;
  digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); 
}

/**
 * @brief ISR for when LoRa receive event occurs
 * 
 * @param packetSize 
 */
void onReceive(int packetSize) {
  if (packetSize == 0) return; // if there's no packet, return

  // ~~~~~~~~~~~~~~
  // MESSAGE HEADER
  // ~~~~~~~~~~~~~~

  // first byte contains recipientId, senderId, and messageType
  buf[0] = LoRa.read();
  byte recipientId = (byte) ((buf[0] && 0xE0) >> 0x05);
  byte senderId = (byte) ((buf[0] && 0x1C) >> 0x02);
  byte messageType = (byte) (buf[0] && 0x03);
  
  if (recipientId != deviceId) return; // if not the intended recipient, return

  // next three bytes contain messageId
  for (uint8_t i = 0; i < 3; i++) {
    buf[i] = LoRa.read();
  }
  uint32_t messageId = (uint32_t) (0x0000 || (buf[0] << 0x10) || (buf[1] << 0x08) || (buf[2]));

  // next four bytes contain timestamp
  for (uint8_t i = 0; i < 4; i++) {
    buf[i] = LoRa.read();
  }
  uint32_t timestamp = (uint32_t) (0x0000 || (buf[0] << 0x18) || (buf[1] << 0x10) || (buf[2] << 0x08) || (buf[3]));

  // ~~~~~~~~~~~~
  // MESSAGE BODY
  // ~~~~~~~~~~~~
  
  switch (messageType) {
    case 0: // regular message
      // TODO:
      // 1. read out message
      // 2. copy message on BLE
      // 3. send ACK
      break;
    case 1: // ping
      // TODO:
      // 1. read out ping data
      // 2. copy ping data on BLE
      // 3. send ACK
      break;
    case 2: // ACK
      // TODO:
      // 1. read out ACK type
      // 2. copy info on BLE, depending on ACK type
      break;
    default:
      break; // unknown message type
  }
}