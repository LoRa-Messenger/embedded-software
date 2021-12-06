#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <TimeLib.h>
#include <CircularBuffer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <TaskScheduler.h>
#include <TinyGPS.h> 
#include "heltec.h"

#include <BLE.h>
#include <LoRaMessage.h>

// Set to 0 if GPS module is not connected
#define GPS_MODULE_PRESENT 0
#define RX_PIN_FOR_GPS 22
#define TX_PIN_FOR_GPS 23

//enable or not the OLED and it's corresponding thread
#define OLED_ENABLE 1

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

#define LORA_HEADER_SIZE 8

// ID of this device to compare with recipientId for incoming messages
byte deviceId = 0x01; // temporary for testing purposes

uint32_t counter = 0; // temporary for testing purposes

//GPS variables
float lat = 28.5458,lon = 77.1703;
uint32_t deviceLatitude = DEFAULT_LATITUDE; //latitude variable multiplied by 1 000 000
uint32_t deviceLongitude = DEFAULT_LONGITUDE; //longitude variable multiplied by 1 000 000
TinyGPS gps; // create gps object 
bool gps_data_fixed;

// locking bools to avoid queues being processed while receiving BLE or LoRA
bool isReceivingBLE = false;
bool isReceivingLoRa = false;
bool toPushBLESendMes = false;

// buffer for reading out incoming message headers
byte buf[4];

int lastLoRaRSSI = 0;
int LastLoRapackSize = 0;
int lastLoRASenderID = 0;

int lastBLERSSI = 0;
int LastBLEMessageSize = 0;

int LoRaReceivedPacketCounter = 0;
int LoRaSentPacketCounter = 0;
int LoRaPingCounter = 0;
int BLEReceivedPacketCounter = 0;
int BLESentPacketCounter = 0;

Scheduler runner;

// use circular buffers to act as queues:
// one for outgoing messages to be sent via LoRa
// one for outgoing messages to be sent via BLE
CircularBuffer<AbstractMessage*, QUEUE_LEN_LORA> LoRaQueue;
CircularBuffer<BLEData*, QUEUE_LEN_BLE> BLEQueue;


void onButtonPress();
void onReceive(int packetSize);
void LoRaDataOLED();
void BLEDataOLED();

// Callback methods prototypes
void t1LoRaCallback();
void t2BLECallback();
void t3GPSCallback();
void t4OLEDCallback();
void t5PingCallback();

//Tasks
//TODO adjust frequencies 
Task t1LoRa(1000, TASK_FOREVER, &t1LoRaCallback);
Task t2BLE(2000, TASK_FOREVER, &t2BLECallback);
Task t3GPS(15000, TASK_FOREVER, &t3GPSCallback);
Task t4OLED(1000, TASK_FOREVER, &t4OLEDCallback);
Task t5Ping(60000, TASK_FOREVER, &t5PingCallback);

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
  Heltec.begin(OLED_ENABLE, true, true, true, BAND);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);

  //Task Scheduler
  runner.init();
  Serial.println("Initialized scheduler");
  
  runner.addTask(t1LoRa);
  runner.addTask(t2BLE);

  t1LoRa.enable();
  Serial.println("Enabled t1LoRa task");
  t2BLE.enable();
  Serial.println("Enabled t2BLE task");

  //GPS module
  if(GPS_MODULE_PRESENT){
    Serial2.begin(9600, SERIAL_8N1, RX_PIN_FOR_GPS, TX_PIN_FOR_GPS);
    runner.addTask(t3GPS);
    t3GPS.enable();
    Serial.println("Enabled t3GPS task");
  }

  //OLED display
  if(OLED_ENABLE){
    runner.addTask(t4OLED);
    t4OLED.enable();
    Serial.println("Enabled t4OLED task");
  }

  runner.addTask(t5Ping);
  t5Ping.enable();
  Serial.println("Enabled t5Ping task");

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
    isReceivingBLE = true;
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
    LoRaQueue.push(packetSend);
    BLEReceivedPacketCounter++;
    toPushBLESendMes = false;
    senCharProcessed->setValue("1");
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
        LoRaSentPacketCounter++;
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
  // Serial.print("BLE queue size: ");
  // Serial.println(BLEQueue.size());
  
  // process first BLE message in queue
  if (!BLEQueue.isEmpty()) {
    if(recCharProcessed->getValue() == "0"){
      Serial.println("BLE data is being processed, will wait next cycle send next item");
    }
    else{
      BLEData *data = BLEQueue.shift();
      recCharProcessed->setValue("0");
      data->updateBLEChars();
      // delete data;
      Serial.println("Updated BLE Charac");
      BLESentPacketCounter++;
    }
  }
}

/**
 * GPS Thread
 */
void t3GPSCallback(){
  int inByte = 0; //for serial data
  while(Serial2.available()){ // check for gps data 
    inByte = Serial2.read();

    //Serial.print((char)inByte); //for debugging
    if(gps.encode(inByte)){// encode gps data
      gps_data_fixed = true;
      gps.f_get_position(&lat,&lon); // get latitude and longitude 
      Serial.printf("Latitude : %f, Longitude: %f \n",lat,lon);
      //update our latitude and longitude
      deviceLatitude = (uint32_t)(lat*GPS_DATA_MULTIPLIER);
      deviceLongitude = (uint32_t)(lon*GPS_DATA_MULTIPLIER);
    }
    else{
      gps_data_fixed = false;
    }
  }
  if(!gps_data_fixed){
    Serial.println("GPS position not fixed yet. Should take less than a minute with clear sky");
  }
}

/**
 * OLED Thread
 */
void t4OLEDCallback(){
  Heltec.display->clear();

  LoRaDataOLED();
  Heltec.display->drawLine(64,0,64,64); //vertical line in the middle
  BLEDataOLED();
}

/**
 * @brief Task for pinging all devices in network
 */
void t5PingCallback() {
  PingMessage *packet = new PingMessage(BROADCAST_ID, deviceId, LoRaPingCounter, (uint32_t) now(), deviceLatitude, deviceLongitude);
  LoRaQueue.push(packet);
  LoRaPingCounter++;
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

  LoRaReceivedPacketCounter++;
  // ~~~~~~~~~~~~~~
  // MESSAGE HEADER
  // ~~~~~~~~~~~~~~

  // first byte contains recipientId, senderId, and messageType
  buf[0] = LoRa.read();
  byte recipientId = (byte) ((buf[0] & 0xE0) >> 0x05);
  byte senderId = (byte) ((buf[0] & 0x1C) >> 0x02);
  byte messageType = (byte) (buf[0] & 0x03);
  
  lastLoRASenderID = (int)senderId;

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

  lastLoRaRSSI = LoRa.packetRssi();
  Serial.printf("[%u]\tPacket received with RSSI [%i]\n", messageId, lastLoRaRSSI);

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
        int message_size_bytes = 0;

        // add bytes one by one
        while (LoRa.available()) {
          incoming += (char) LoRa.read();
          message_size_bytes++;
        }

        LastLoRapackSize = message_size_bytes + LORA_HEADER_SIZE;

        Serial.printf("[%u]\tMessage: ", messageId);
        Serial.println(incoming.c_str());

        // Put Acknowledge on LoRa Queue
        // needs to use the 'new' operator to dynamically allocate memory,
        // as we want to add the pointer to a queue of AbstractMessage pointers
        ReceivedACK *packet = new ReceivedACK(senderId, deviceId, messageId, (uint32_t) now());
        LoRaQueue.push(packet);

        // Put message on BLE Queue
        BLEData *data = new BLEData(recipientId, senderId, messageId, timestamp, deviceLatitude, deviceLongitude, incoming);
        BLEQueue.push(data);

        break;
      }

    case PING_MESSAGE:
      {
        // next four bytes contain latitude
        for (uint8_t i = 0; i < 4; i++) {
          buf[i] = LoRa.read();
        }
        int32_t latitude = (int32_t) (0x0000 | (buf[0] << 0x18) | (buf[1] << 0x10) | (buf[2] << 0x08) | (buf[3]));
        // float latitude = (float) latitude_int / GPS_DATA_MULTIPLIER;

        // next four bytes contain longitude
        for (uint8_t i = 0; i < 4; i++) {
          buf[i] = LoRa.read();
        }
        int32_t longitude = (int32_t) (0x0000 | (buf[0] << 0x18) | (buf[1] << 0x10) | (buf[2] << 0x08) | (buf[3]));
        // float longitude = (float) longitude_int / GPS_DATA_MULTIPLIER;

        Serial.printf("[%u]\tLatitude: [%d], Longitude: [%d]\n", messageId, latitude, longitude);

        // Put Acknowledge on LoRa Queue
        // needs to use the 'new' operator to dynamically allocate memory,
        // as we want to add the pointer to a queue of AbstractMessage pointers
        PingACK *packet = new PingACK(senderId, deviceId, messageId, (uint32_t) now());
        LoRaQueue.push(packet);

        // TODO: send BLE data
        // Put message on BLE Queue
        // BLEData *data = new BLEData(recipientId, senderId, messageId, timestamp, latitude_int, longitude_int, "PING");
        // BLEQueue.push(data);
        
        break;
      }
    case ACK_MESSAGE:
      // TODO
      break;
    default:
      break; // unknown message type
  }
  isReceivingLoRa = false;
}


void LoRaDataOLED(){
  Heltec.display->drawStringMaxWidth(0, 0, 60, "LoRa"); 
  Heltec.display->drawStringMaxWidth(0, 13, 60, "RSSI:" + String(lastLoRaRSSI)); 
  Heltec.display->drawStringMaxWidth(0 , 24 , 60, "bytes:" + String(LastLoRapackSize));
  Heltec.display->drawStringMaxWidth(0 , 35 , 60, "S ID:" + String(lastLoRASenderID));
  Heltec.display->drawStringMaxWidth(0 , 46 , 60, "R:" + String(LoRaReceivedPacketCounter) + 
                                                  " | T:" + String(LoRaSentPacketCounter));
  Heltec.display->display();
}


void BLEDataOLED(){
  Heltec.display->drawStringMaxWidth(68, 0, 60, "BLE"); 
  // Heltec.display->drawStringMaxWidth(68, 13, 60, "RSSI: " + String(lastLoRaRSSI)); 
  // Heltec.display->drawStringMaxWidth(68 , 24 , 60, "Received "+ String(LastLoRapackSize + " bytes"));
  // Heltec.display->drawStringMaxWidth(68 , 35 , 60, "Sender ID " + String(lastLoRASenderID));
  Heltec.display->drawStringMaxWidth(68 , 46 , 60, "R:" + String(BLEReceivedPacketCounter) + 
                                                  " | T:" + String(BLESentPacketCounter));
  Heltec.display->display();
}