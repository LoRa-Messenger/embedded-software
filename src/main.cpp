#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <LoRaMessage.h>
#include <TimeLib.h>
#include <CircularBuffer.h>

#include "heltec.h"

#define BAND 915E6 // LoRa frequency band
#define BUTTON 0
#define LED 25

#define QUEUE_LEN 10

// ID of this device to compare with recipientId for incoming messages
byte deviceId = 0x01; // temporary for testing purposes

uint32_t counter = 0; // temporary for testing purposes

// buffer for reading out incoming message headers
byte buf[4];

// use circular buffers to act as queues:
// one for outgoing messages to be sent via LoRa
// one for outgoing messages to be sent via BLE
CircularBuffer<AbstractMessage*, QUEUE_LEN> LoRaQueue;
CircularBuffer<AbstractMessage*, QUEUE_LEN> BLEQueue;

void onButtonPress();
void onReceive(int packetSize);

void setup() {
  // setup the pins
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);

  // register the pushbutton ISR
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
  // process first LoRa message in queue
  if (!LoRaQueue.isEmpty()) {
    AbstractMessage *message = LoRaQueue.shift();

    digitalWrite(LED, HIGH);
    Serial.printf("[%u]\tSending packet...\n", message->getMessageId());

    unsigned long start = millis();
    if (message->sendPacket()) {
      unsigned long end = millis();
      Serial.printf("[%u]\tPacket sent in [%u] ms\n", message->getMessageId(), end - start);
    } else {
      Serial.printf("[%u]\tFailed to send packet\n", message->getMessageId());
    }

    digitalWrite(LED, LOW);
  }
}

/**
 * @brief ISR for button press that will flag for a "hello world" LoRa packet to be sent.
 * Note that sending the message from the ISR itself takes too long, and thus will cause the system watchdog timer to freak out,
 * causing the system to crash.
 */
void onButtonPress() {
  RegularMessage packet((byte) BROADCAST_ID, deviceId, counter, (uint32_t) now(), "hello world");
  LoRaQueue.push(&packet);
  counter++;
}

/**
 * @brief Callback for when LoRa receive event occurs
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
    case REGULAR_MESSAGE:
      {
        String incoming = ""; // payload of packet

        while (LoRa.available()) {
          incoming += (char)LoRa.read(); // add bytes one by one
        }

        Serial.printf("[%u]\tMessage: ", messageId);
        Serial.println("[" + incoming + "]");

        ReceivedACK packet(senderId, deviceId, messageId, (uint32_t) now());
        LoRaQueue.push(&packet);

        // sendAckFlag = true;
        break;
      }

    case PING_MESSAGE:
      // TODO
      break;
    case ACK_MESSAGE:
      // TODO
      break;
    default:
      break; // unknown message type
  }
}