#define DEF__ABSTRACTMESSAGE_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <AbstractMessage.h>
#endif

#include <AbstractMessage.h>

using namespace std;

AbstractMessage::AbstractMessage(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp) {
  this->recipientId = recipientId;
  this->senderId = senderId;
  this->messageId = messageId;
  this->timestamp = timestamp;  
}

int AbstractMessage::getRecipientId() {
  return this->recipientId;
}

int AbstractMessage::getSenderId() {
  return this->senderId;
}

int AbstractMessage::getMessageId() {
  return this->messageId;
}

int AbstractMessage::getTimestamp() {
  return this->timestamp;
}