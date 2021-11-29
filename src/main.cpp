#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <LoRaMessage.h>
#include <TimeLib.h>
#include "heltec.h"

#define BAND 915E6 // LoRa frequency band
#define BUTTON 0
#define LED 25

// ID of this device to compare with recipientId for incoming messages
byte deviceId = 0x01; // temporary for testing purposes

uint32_t counter = 0; // temporary for testing purposes

// buffer for reading out incoming message headers
byte buf[4];

bool sendMsgFlag = false;
bool sendAckFlag = false;
byte lastSenderId;
uint32_t lastMessageId;

void onButtonPress();
void onReceive(int packetSize);

void setup() {
  // TODO:
  // 1. connect with phone over BLE
  // 2. initialize all the device IDs (so they are all in agreement as to who is who)

  // TODO: setup button press ISR to transmit LoRa message
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON), onButtonPress, RISING);

  // enable OLED display, LoRa, UART serial, and PABOOST; set LoRa frequency band
  // note that this sets serial baudrate to 115200
  Heltec.begin(true, true, true, true, BAND);

  // register the receive callback
  LoRa.onReceive(onReceive);

  // put the radio into receive mode
  LoRa.receive();
}

void loop() {
  // TODO:
  // 1. if button is pressed, send "hello world"

  if (sendMsgFlag) {
    Serial.printf("[%u]\tSending packet...\n", counter);
    unsigned long start = millis();
    String hello = "hello world";
    RegularMessage message((byte) BROADCAST_ID, deviceId, counter, (uint32_t) now(), hello);
    int status = message.sendPacket();
    unsigned long end = millis();
    LoRa.receive();
    if (status) Serial.printf("[%u]\tSent packet in [%u] ms\n", counter, end - start);
    else Serial.printf("[%u]\tFailed to send packet\n", counter);
    counter++;
    digitalWrite(LED, LOW);
    
    sendMsgFlag = false;
  }
  if (sendAckFlag) {
    Serial.printf("[%u]\tSending ACK packet...\n", lastMessageId);
    ReadACK ack((byte) lastSenderId, deviceId, lastMessageId, (uint32_t) now());
    ack.sendPacket();
    LoRa.receive();
    digitalWrite(LED, LOW);

    sendAckFlag = false;
  }
}

void onButtonPress() {
  // 1. turn on LED
  // 2. print to terminal
  // 3. construct a RegularMessage
  // 4. transmit the message
  // 5. return the LoRa radio back into receive mode
  // 6. increment messageId
  // 7. turn off LED
  sendMsgFlag = true;
  digitalWrite(LED, HIGH);

  // Serial.printf("[%u]\tSending packet...\n", counter);
  // RegularMessage message((byte) 0x00, deviceId, counter, (uint32_t) now(), "hello world");
  // message.sendPacket();
  // LoRa.receive();
  // counter++;
  // digitalWrite(LED, LOW);
}

/**
 * @brief Callback for when LoRa receive event occurs
 * 
 * @param packetSize 
 */
void onReceive(int packetSize) {
  // Serial.printf("[%u]\tPacket size [%i]\n", packetSize);
  if (packetSize == 0) return; // if there's no packet, return

  // ~~~~~~~~~~~~~~
  // MESSAGE HEADER
  // ~~~~~~~~~~~~~~

  // first byte contains recipientId, senderId, and messageType
  buf[0] = LoRa.read();
  byte recipientId = (byte) ((buf[0] & 0xE0) >> 0x05);
  byte senderId = (byte) ((buf[0] & 0x1C) >> 0x02);
  byte messageType = (byte) (buf[0] & 0x03);
  
  if (recipientId != deviceId && recipientId != BROADCAST_ID) return; // if not the intended recipient, return

  // next three bytes contain messageId
  for (uint8_t i = 0; i < 3; i++) {
    buf[i] = LoRa.read();
  }
  uint32_t messageId = (uint32_t) (0x0000 | (buf[0] << 0x10) | (buf[1] << 0x08) | (buf[2]));

  Serial.printf("[%u]\tPacket received with RSSI [%i]\n", messageId, LoRa.packetRssi());

  // next four bytes contain timestamp
  for (uint8_t i = 0; i < 4; i++) {
    buf[i] = LoRa.read();
  }
  uint32_t timestamp = (uint32_t) (0x0000 | (buf[0] << 0x18) | (buf[1] << 0x10) | (buf[2] << 0x08) | (buf[3]));

  Serial.printf("[%u]\tPacket arrived in [%i] seconds\n", messageId, (uint32_t) now() - timestamp);

  // ~~~~~~~~~~~~
  // MESSAGE BODY
  // ~~~~~~~~~~~~
  
  switch (messageType) {
    case REGULAR_MESSAGE: // regular message
      {
        // TODO:
        // 1. read out message
        // 2. copy message on BLE
        // 3. send ACK
        String incoming = ""; // payload of packet

        while (LoRa.available()) {
          incoming += (char)LoRa.read(); // add bytes one by one
        }

        Serial.printf("[%u]\tMessage: ", messageId);
        Serial.println(incoming);
        sendAckFlag = true;
        digitalWrite(LED, HIGH);
        // Serial.printf("[%u]\tSending ACK packet...\n", messageId);
        // ReadACK ack((byte) senderId, deviceId, messageId, (uint32_t) now());
        // ack.sendPacket();
        // LoRa.receive();
        // digitalWrite(LED, LOW);
        break;
      }

    case PING_MESSAGE: // ping
      // TODO:
      // 1. read out ping data
      // 2. copy ping data on BLE
      // 3. send ACK
      break;
    case ACK_MESSAGE: // ACK
      // TODO:
      // 1. read out ACK type
      // 2. copy info on BLE, depending on ACK type
      break;
    default:
      break; // unknown message type
  }
}