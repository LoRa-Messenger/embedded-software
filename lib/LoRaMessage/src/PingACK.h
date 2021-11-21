#ifndef DEF__PINGACK_H
#define DEF__PINGACK_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "PingACK.h"
#endif
#endif

#include <AbstractACK.h>

using namespace std;

class PingACK : public AbstractACK {
	
	public:
        /**
         * @brief Construct a new Ping ACK object
         * 
         * @param recipientId 
         * @param senderId 
         * @param messageId 
         * @param timestamp 
         */
		PingACK(const int recipientId, const int senderId, const int messageId, const int timestamp);

        int sendPacket();

};

#endif