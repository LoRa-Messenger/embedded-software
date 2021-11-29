#ifndef DEF__ABSTRACTACK_H
#define DEF__ABSTRACTACK_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "AbstractACK.h"
#endif
#endif

#include <AbstractMessage.h>

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