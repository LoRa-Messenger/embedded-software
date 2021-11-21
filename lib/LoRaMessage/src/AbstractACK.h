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
		AbstractACK(const int recipientId, const int senderId, const int messageId, const int timestamp);

        virtual int sendPacket() = 0;

};

#endif