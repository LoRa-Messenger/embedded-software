// #ifndef BLE_H
// #define BLE_H


#include <BLEDevice.h>
#include <Arduino.h>

#define DEVICE_BLE_NAME "Juan"

#define SERVICE_SEND_UUID                 "0f155954-2316-45d0-8791-c18247c6c146"
#define SERVICE_RECEIVE_UUID              "0f0e363b-2cdb-480b-ab3c-e5e784cf68c8"

#define SEN_CHARACTERISTIC_UUID_SEN_ID    "6472961c-3e23-428e-b1f7-efd1e44974b7"
#define SEN_CHARACTERISTIC_UUID_REC_ID    "4a874290-53ea-4f5e-a07b-26a6e9916306" 
#define SEN_CHARACTERISTIC_UUID_MES_ID    "fd369597-96ea-4f73-8ac7-4e4111122fd4"
#define SEN_CHARACTERISTIC_UUID_TEXT      "cab0c824-dc00-4257-8876-f3d8e8cc26dd"
#define SEN_CHARACTERISTIC_UUID_PROCESSED "c78b01d2-15ce-43bd-b3f6-efcdf2f8fc37"

#define REC_CHARACTERISTIC_UUID_SEN_ID    "adeba5b5-d69d-4a26-b91e-1dea9aebfd42"
#define REC_CHARACTERISTIC_UUID_REC_ID    "0a8f5e85-fa36-405b-97fa-5a8b485c2961" 
#define REC_CHARACTERISTIC_UUID_MES_ID    "2658e4b8-6c0f-443f-9289-b5dc1fde1ab2"
#define REC_CHARACTERISTIC_UUID_TIME      "ee91aade-5bb7-404d-aa58-db8573086597"
#define REC_CHARACTERISTIC_UUID_LAT       "c059e6c0-4801-4db3-a34f-54c90c026490" 
#define REC_CHARACTERISTIC_UUID_LONG      "b61da9aa-3279-4459-ac08-d22dc05c37c0" 
#define REC_CHARACTERISTIC_UUID_TEXT      "14868cc4-40dd-4369-aae3-962ef8887b26"
#define REC_CHARACTERISTIC_UUID_PROCESSED "7d375b33-ab1d-46bd-a71f-212de697cf12"

#define DEFAULT_ID      "00"
#define DEFAULT_TIME    "1638118131"
#define DEFAULT_LAT     "45.5648"
#define DEFAULT_LONG    "-73.56821"
#define DEFAULT_MESS    "Default message"


class BLEData {
	protected:
		byte recipientId;
		byte senderId;
		uint32_t messageId;
		uint32_t timestamp;
    uint32_t latitude;
    uint32_t longitude;
    std::string text;
	
	public:
		/**
		 * @brief Construct a new BLE data object
		 * 
		 * @param recipientId 
		 * @param senderId 
		 * @param messageId 
		 * @param timestamp
         * @param latitude 
         * @param longitude 
         * @param text  
		 */
		BLEData(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp, \
                 const uint32_t latitude, const uint32_t longitude, const std::string text);
		
		/**
		 * @brief updates the BLE Characteristics with this objects infos 
		 */
    void updateBLEChars();

    //Getters
    byte getRecipientId();
    byte getSenderId();
    uint32_t getMessageId();
    uint32_t getTimestamp();
    uint32_t getLatitude();
    uint32_t getLongitude();
    std::string getText();

    //Setters
    void setRecipientId(byte recipientId);
    void setSenderId(byte senderId);
    void setMessageId(uint32_t messageId);
    void setTimestamp(uint32_t timestamp);
    void setLatitude(uint32_t latitude);
    void setLongitude(uint32_t longitude);
    void setText(std::string text);
};

void BLEAdvertise();
void BLEConfig();

extern BLECharacteristic *recCharSendID, *recCharRecID, *recCharMesID, *recCharTime, *recCharLat, *recCharLong, *recCharText, *recCharProcessed;
extern BLECharacteristic *senCharSendID, *senCharRecID, *senCharMesID, *senCharText, *senCharProcessed;

extern bool toPushBLESendMes;
// #endif
































