#ifndef DEF__ABSTRACTMESSAGE_H
#define DEF__ABSTRACTMESSAGE_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "AbstractMessage.h"
#endif
#endif

#include <Arduino.h>

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