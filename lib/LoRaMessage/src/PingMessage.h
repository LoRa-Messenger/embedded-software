#ifndef DEF__PINGMESSAGE_H
#define DEF__PINGMESSAGE_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "PingMessage.h"
#endif
#endif

#include <AbstractMessage.h>

/**
 * @brief For latitude and longitude, floats don't work for bitwise operations easily,
 * so we multiply them by 1E6 to get 6 decimal places of precision,
 * then round and cast to signed int, which we can transmit easily.
 */
#define GPS_DATA_MULTIPLIER 1000000 // 1,000,000
#define LORA_PING_BODY_SIZE 8

using namespace std;

class PingMessage : public AbstractMessage{
	
	protected:
		int latitude;
        int longitude;
	
	public:
    /**
     * @brief Construct a new Ping Message object
     * 
     * @param recipientId 
     * @param senderId 
     * @param messageId 
     * @param timestamp 
     * @param latitude 
     * @param longitude 
     */
    PingMessage(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp, const int32_t latitude, const int32_t longitude);

    int sendPacket();

    int getLatitude();
    int getLongitude();

};

#endif