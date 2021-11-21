#ifndef DEF__PINGMESSAGE_H
#define DEF__PINGMESSAGE_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "PingMessage.h"
#endif
#endif

#include <AbstractMessage.h>

using namespace std;

class PingMessage : public AbstractMessage{
	
	protected:
		float latitude;
        float longitude;
	
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
		PingMessage(const int recipientId, const int senderId, const int messageId, const int timestamp, const float latitude, const float longitude);

        int sendPacket();

};

#endif