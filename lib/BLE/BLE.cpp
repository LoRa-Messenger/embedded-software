#include "BLE.h"
#include "heltec.h"
#include <BLE2902.h>

BLEServer *pServer;
BLEService *sendService, *receiveService;
BLECharacteristic *recCharSendID, *recCharRecID, *recCharMesID, *recCharTime, *recCharLat, *recCharLong, *recCharText, *recCharProcessed;
BLECharacteristic *senCharSendID, *senCharRecID, *senCharMesID, *senCharText, *senCharProcessed;

//class for queue
BLEData::BLEData(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp, \
                 const uint32_t latitude, const uint32_t longitude, const std::string text) {
  this->recipientId = recipientId;
  this->senderId = senderId;
  this->messageId = messageId;
  this->timestamp = timestamp;
  this->latitude = latitude;
  this->longitude = longitude;
  this->text = text;  
}

//Getters
byte BLEData::getRecipientId() {return this->recipientId;}
byte BLEData::getSenderId() {return this->senderId;}
uint32_t BLEData::getMessageId() {return this->messageId;}
uint32_t BLEData::getTimestamp() {return this->timestamp;}
uint32_t BLEData::getLatitude() {return this->latitude;}
uint32_t BLEData::getLongitude() {return this->longitude;}
std::string BLEData::getText() {return this->text;}

//Setters
void BLEData::setRecipientId(byte recipientId) {this->recipientId = recipientId;}
void BLEData::setSenderId(byte senderId) {this->senderId = senderId;}
void BLEData::setMessageId(uint32_t messageId) {this->messageId = messageId;}
void BLEData::setTimestamp(uint32_t timestamp) {this->timestamp = timestamp;}
void BLEData::setLatitude(uint32_t latitude) {this->latitude = latitude;}
void BLEData::setLongitude(uint32_t longitude) {this->longitude = longitude;}
void BLEData::setText(std::string text) {this->text = text;}

void BLEData::updateBLEChars(){
  //TODO data type conversions need to happen here. convert all to ASCII string
  uint16_t senderID_uint16 = (0 << 8) | ((uint8_t)this->senderId);
  recCharSendID->setValue(senderID_uint16);
  // recCharRecID->setValue((uint32_t&)this->senderId);
  recCharTime->setValue(this->timestamp);
  Serial.printf("Time: %u\n",this->timestamp);
  // recCharLat->setValue(this->latitude);
  // recCharLong->setValue(this->longitude);
  recCharText->setValue(this->text);
  recCharMesID->setValue(this->messageId);
  recCharMesID->notify();
  Serial.println("got here0");
}


class MyCharCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacterstic){
    senCharProcessed->setValue("0");
    // Serial.println("onWriteCallback");
    // std::string valueRec = senCharText->getValue();
    // Serial.println(valueRec.c_str());
    // std::string valueRec2 = pCharacterstic->getValue();
    // Serial.println(valueRec2.c_str()); 
    toPushBLESendMes = true;
  }
};

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer){
    Serial.println("Bluetooth Device Connected");
    //TODO enable BLE task here
  }
  void onDisconnect(BLEServer* pServer){
    Serial.println("Bluetooth Disconnected");
    Serial.println("Restarting Advertising");
    BLEAdvertise();
    //TODO disable BLE task here
  }
};


void BLEAdvertise(){
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_SEND_UUID);
  pAdvertising->addServiceUUID(SERVICE_RECEIVE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  // Serial.println("Characteristic defined! Now you can read it in the Client!");
}

void BLEConfig(){
  BLEDevice::init(DEVICE_BLE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // recServer = BLEDevice::createServer();
  // recServer->setCallbacks(new MyServerCallbacks());

  sendService = pServer->createService(SERVICE_SEND_UUID);
  receiveService = pServer->createService(SERVICE_RECEIVE_UUID);

  //send characteristics
  senCharSendID = sendService->createCharacteristic(SEN_CHARACTERISTIC_UUID_SEN_ID, BLECharacteristic::PROPERTY_READ |BLECharacteristic::PROPERTY_WRITE);
  senCharRecID = sendService->createCharacteristic(SEN_CHARACTERISTIC_UUID_REC_ID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  senCharMesID = sendService->createCharacteristic(SEN_CHARACTERISTIC_UUID_MES_ID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  senCharText = sendService->createCharacteristic(SEN_CHARACTERISTIC_UUID_TEXT, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  senCharProcessed = sendService->createCharacteristic(SEN_CHARACTERISTIC_UUID_PROCESSED, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  //receive characteristics 
  //Some reason max 6 characteristics
  recCharText = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_TEXT, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  recCharProcessed = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_PROCESSED, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  recCharSendID = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_SEN_ID, BLECharacteristic::PROPERTY_READ |BLECharacteristic::PROPERTY_NOTIFY);
  // recCharRecID = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_REC_ID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  recCharMesID = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_MES_ID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  recCharTime = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_TIME, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  // recCharLat = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_LAT, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  // recCharLong = receiveService->createCharacteristic(REC_CHARACTERISTIC_UUID_LONG, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  //set default values
  // senCharSendID->setValue(DEFAULT_ID);
  senCharRecID->setValue(DEFAULT_ID);
  senCharMesID->setValue(DEFAULT_ID);
  senCharText->setValue(DEFAULT_MESS);
  senCharProcessed->setValue("1");

  recCharText->setValue(DEFAULT_MESS);
  recCharProcessed->setValue("1");
  recCharSendID->setValue(DEFAULT_ID);
  // recCharRecID->setValue(DEFAULT_ID);
  recCharMesID->setValue(DEFAULT_ID);
  recCharMesID->addDescriptor(new BLE2902());
  recCharTime->setValue(DEFAULT_TIME);
  // recCharLat->setValue(DEFAULT_LAT);
  // recCharLong->setValue(DEFAULT_LONG);


  //set callback for message ID
  senCharMesID->setCallbacks(new MyCharCallback());


  //start services
  sendService->start();
  receiveService->start();
}