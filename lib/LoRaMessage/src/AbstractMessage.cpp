#define DEF__ABSTRACTMESSAGE_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <AbstractMessage.h>
#endif

#include <AbstractMessage.h>

using namespace std;

AbstractMessage::AbstractMessage(const int recipientId, const int senderId, const int messageId, const int timestamp) {
  this->recipientId = recipientId;
  this->senderId = senderId;
  this->messageId = messageId;
  this->timestamp = timestamp;  
}