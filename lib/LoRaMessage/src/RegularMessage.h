#ifndef DEF__REGULARMESSAGE_H
#define DEF__REGULARMESSAGE_H

#ifdef PRAGMA
#pragma once
#ifndef _MSC_VER
#pragma interface "RegularMessage.h"
#endif
#endif

#include <AbstractMessage.h>
#include <string>

using namespace std;

class RegularMessage : public AbstractMessage{
	
	protected:
		string message;
	
	public:
		/**
		 * @brief Construct a new Regular Message object
		 * 
		 * @param recipientId 
		 * @param senderId 
		 * @param messageId 
		 * @param timestamp 
		 * @param message 
		 */
		RegularMessage(const int recipientId, const int senderId, const int messageId, const int timestamp, const string message);

        int sendPacket();

};

#endif