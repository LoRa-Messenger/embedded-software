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

byte AbstractMessage::getRecipientId() {
  return this->recipientId;
}

byte AbstractMessage::getSenderId() {
  return this->senderId;
}

uint32_t AbstractMessage::getMessageId() {
  return this->messageId;
}

uint32_t AbstractMessage::getTimestamp() {
  return this->timestamp;
}