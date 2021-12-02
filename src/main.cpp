#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <LoRaMessage.h>
#include <TimeLib.h>
#include <CircularBuffer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <TaskScheduler.h>

#include "heltec.h"
#include <BLE.h>

// LoRa frequency band = 915 MHz
#define BAND 915E6

//McGill location
#define DEFAULT_LATITUDE 45506152 // multiplied by 1,000,000
#define DEFAULT_LONGITUDE -73576416// multiplied by 1,000,000

// pin IDs
#define BUTTON 0
#define LED 25

#define QUEUE_LEN_BLE   10
#define QUEUE_LEN_LORA  20

// ID of this device to compare with recipientId for incoming messages
byte deviceId = 0x01; // temporary for testing purposes

uint32_t counter = 0; // temporary for testing purposes

uint32_t latitude = DEFAULT_LATITUDE; 
uint32_t longitude = DEFAULT_LONGITUDE;

// locking bools to avoid queues being processed while receiving BLE or LoRA
bool isReceivingBLE = false;
bool isReceivingLoRa = false;
bool toPushBLESendMes = false;

// buffer for reading out incoming message headers
byte buf[4];

String rssi = "RSSI --";
String packSize = "--";
String packet ;

Scheduler runner;

// use circular buffers to act as queues:
// one for outgoing messages to be sent via LoRa
// one for outgoing messages to be sent via BLE
CircularBuffer<AbstractMessage*, QUEUE_LEN_LORA> LoRaQueue;
CircularBuffer<BLEData*, QUEUE_LEN_BLE> BLEQueue;

void onButtonPress();
void onReceive(int packetSize);
void LoRaDataOLED();

// Callback methods prototypes
void t1LoRaCallback();
void t2BLECallback();
void t3GPSCallback();

//Tasks
//TODO adjust frequencies 
Task t1LoRa(1000, TASK_FOREVER, &t1LoRaCallback);
Task t2BLE(2000, TASK_FOREVER, &t2BLECallback);
Task t3GPS(10000, TASK_FOREVER, &t3GPSCallback);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Server!");
 
  //BLE setup
  BLEConfig();

  //start advertising
  BLEAdvertise();

  // setup the pins
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);

  // register the pushbutton ISR
  attachInterrupt(digitalPinToInterrupt(BUTTON), onButtonPress, RISING);

  // enable OLED display, LoRa, UART serial, and PABOOST; set LoRa frequency band
  // note that this sets serial baudrate to 115200
  Heltec.begin(true, true, true, true, BAND);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);

  //Task Scheduler
  runner.init();
  Serial.println("Initialized scheduler");
  
  runner.addTask(t1LoRa);
  runner.addTask(t2BLE);
  runner.addTask(t3GPS);

  t1LoRa.enable();
  Serial.println("Enabled t1LoRa task");
  t2BLE.enable();
  Serial.println("Enabled t2BLE task");
  t3GPS.enable();
  Serial.println("Enabled t3GPS task");

  //LoRa
  // register the receive callback
  LoRa.onReceive(onReceive);

  // put the radio into receive mode
  LoRa.receive();
}

void loop() {
  runner.execute();
}


/**
 * LoRa Queue Thread
 */
void t1LoRaCallback(){
  //if message sent via BLE, put it on the LoRa Queue
  if(toPushBLESendMes){
    uint32_t test = atoi(senCharRecID->getValue().c_str());
    uint32_t test2 = atoi(senCharMesID->getValue().c_str());
    Serial.printf("test1 %u\n", test);
    Serial.printf("test2 %u\n", test2);
    //TODO verify the type conversions here
    RegularMessage *packetSend = new RegularMessage(atoi(senCharRecID->getValue().c_str()), 
                                                atoi(senCharRecID->getValue().c_str()), 
                                                atoi(senCharMesID->getValue().c_str()), 
                                                (uint32_t) now(),
                                                senCharText->getValue());
    Serial.printf("got here0 %u\n");
    LoRaQueue.push(packetSend);
    Serial.printf("got here1 %u\n");
    toPushBLESendMes = false;
  }

  // process first LoRa message in queue
  if (!LoRaQueue.isEmpty()) {
    if(isReceivingLoRa){
      Serial.println("LoRa being received, will wait next cycle to process queue");
    }
    else{
      AbstractMessage *packet = LoRaQueue.shift();

      digitalWrite(LED, HIGH);
      Serial.printf("[%u]\tSending packet...\n", packet->getMessageId());

      unsigned long start = millis();
      if (packet->sendPacket()) {
        unsigned long end = millis();
        Serial.printf("[%u]\tPacket sent in [%u] ms\n", packet->getMessageId(), end - start);
        delete packet;
      } else {
        Serial.printf("[%u]\tFailed to send packet\n", packet->getMessageId());
      }

      digitalWrite(LED, LOW);

      // put the LoRa radio back in receive mode
      LoRa.receive();
    }
  }
  // Serial.println("LoRa thread");
}

/**
 * BLE Queue Thread
 */
void t2BLECallback(){
  if(isReceivingBLE){
    Serial.println("BLE is being received, will wait next cycle to process queue");
  }
  else{
    // process first BLE message in queue
    if (!BLEQueue.isEmpty()) {
      BLEData *data = BLEQueue.shift();
      data->updateBLEChars();
      delete data;
      Serial.println("Updated BLE Charac");
    }
  }
  // Serial.println("BLE thread");
}

/**
 * GPS Thread
 */
void t3GPSCallback(){
    Serial.println("Updating GPS data...");
    //TODO

}



/**
 * @brief ISR for button press that will flag for a "hello world" LoRa packet to be sent.
 * Note that sending the message from the ISR itself takes too long, and thus will cause the system watchdog timer to freak out,
 * causing the system to crash.
 */
void onButtonPress() {
  std::string hello = "hello world";

  // needs to use the 'new' operator to dynamically allocate memory,
  // as we want to add the pointer to a queue of AbstractMessage pointers
  RegularMessage *packet = new RegularMessage((byte) BROADCAST_ID, deviceId, counter, (uint32_t) now(), hello);
  LoRaQueue.push(packet);

  counter++;
}


/**
 * @brief ISR for when LoRa receive event occurs
 * 
 * @param packetSize 
 */
void onReceive(int packetSize) {
  isReceivingLoRa = true;
  // if there's no packet, return
  if (packetSize == 0) return;

  // ~~~~~~~~~~~~~~
  // MESSAGE HEADER
  // ~~~~~~~~~~~~~~

  // first byte contains recipientId, senderId, and messageType
  buf[0] = LoRa.read();
  byte recipientId = (byte) ((buf[0] & 0xE0) >> 0x05);
  byte senderId = (byte) ((buf[0] & 0x1C) >> 0x02);
  byte messageType = (byte) (buf[0] & 0x03);
  
  // if not the intended recipient, return
  if (recipientId != deviceId && recipientId != BROADCAST_ID){ //TODO check this condition
    Serial.printf("Received LoRa Packet but not this device's recipient ID: %x\n",recipientId);
    return;
  } 

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
        // payload of packet
        std::string incoming = "";

        // add bytes one by one
        while (LoRa.available()) {
          incoming += (char) LoRa.read(); 
        }

        Serial.printf("[%u]\tMessage: ", messageId);
        Serial.println(incoming.c_str());

        // Put Acknowledge on LoRa Queue
        // needs to use the 'new' operator to dynamically allocate memory,
        // as we want to add the pointer to a queue of AbstractMessage pointers
        ReceivedACK *packet = new ReceivedACK(senderId, deviceId, messageId, (uint32_t) now());
        LoRaQueue.push(packet);

        // Put message on BLE Queue
        BLEData *data = new BLEData(recipientId, senderId, messageId, (uint32_t) now(), latitude, longitude, incoming);
        BLEQueue.push(data);

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
  isReceivingLoRa = false;
}


// void LoRaDataOLED(){
//   Heltec.display->clear();
//   Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
//   Heltec.display->setFont(ArialMT_Plain_10);
//   Heltec.display->drawString(0 , 15 , "Received "+ packSize + " bytes");
//   Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);
//   Heltec.display->drawString(0, 0, rssi);  
//   Heltec.display->display();
// }