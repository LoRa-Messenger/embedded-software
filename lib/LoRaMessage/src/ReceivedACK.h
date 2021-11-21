#ifndef DEF__RECEIVEDACK_H
#define DEF__RECEIVEDACK_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "ReceivedACK.h"
#endif
#endif

#include <AbstractACK.h>

using namespace std;

class ReceivedACK : public AbstractACK {
	
	public:
        /**
         * @brief Construct a new Received ACK object
         * 
         * @param recipientId 
         * @param senderId 
         * @param messageId 
         * @param timestamp 
         */
		ReceivedACK(const int recipientId, const int senderId, const int messageId, const int timestamp);

        int sendPacket();

};

#endif