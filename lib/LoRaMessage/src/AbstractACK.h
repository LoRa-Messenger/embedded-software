#ifndef DEF__ABSTRACTACK_H
#define DEF__ABSTRACTACK_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "AbstractACK.h"
#endif
#endif

#include <AbstractMessage.h>

#define RECEIVED_ACK 0x00
#define READ_ACK 0x01
#define PING_ACK 0x02

#define LORA_ACK_BODY_SIZE 1

using namespace std;

class AbstractACK : public AbstractMessage{
	
	public:
    /**
     * @brief Construct a new Abstract ACK object
     * 
     * @param recipientId 
     * @param senderId 
     * @param messageId 
     * @param timestamp 
     */
		AbstractACK(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp);

    virtual int sendPacket() = 0;

};

#endif