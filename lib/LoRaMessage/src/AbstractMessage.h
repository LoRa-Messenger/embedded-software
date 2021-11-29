#ifndef DEF__ABSTRACTMESSAGE_H
#define DEF__ABSTRACTMESSAGE_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "AbstractMessage.h"
#endif
#endif

#include <Arduino.h>
#include "heltec.h"

#define REGULAR_MESSAGE 0x00
#define PING_MESSAGE 0x01
#define ACK_MESSAGE 0x02
#define BROADCAST_ID 0x00

using namespace std;

class AbstractMessage {
	
	protected:
		int recipientId;
		int senderId;
		int messageId;
		int timestamp;
	
	public:
		/**
		 * @brief Construct a new Abstract Message object
		 * 
		 * @param recipientId 
		 * @param senderId 
		 * @param messageId 
		 * @param timestamp 
		 */
		AbstractMessage(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp);
		
		/**
		 * @brief 
		 * 
		 * @return int result of the LoRa.endPacket()
		 */
    virtual int sendPacket() = 0;
	
};

#endif