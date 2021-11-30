#ifndef DEF__READACK_H
#define DEF__READACK_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "ReadACK.h"
#endif
#endif

#include <AbstractACK.h>

using namespace std;

class ReadACK : public AbstractACK {
	
	public:
        /**
         * @brief Construct a new Read ACK object
         * 
         * @param recipientId 
         * @param senderId 
         * @param messageId 
         * @param timestamp 
         */
        ReadACK(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp);

        int sendPacket();

};

#endif