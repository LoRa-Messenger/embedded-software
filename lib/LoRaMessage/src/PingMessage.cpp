#define DEF__PINGMESSAGE_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <PingMessage.h>
#endif

#include <PingMessage.h>
#include <AbstractMessage.h>

using namespace std;

PingMessage::PingMessage(const int recipientId, const int senderId, const int messageId, const int timestamp, const float latitude, const float longitude) : AbstractMessage(recipientId, senderId, messageId, timestamp) {
  this->latitude = latitude;
  this->longitude = longitude;  
}

int PingMessage::sendPacket() {
    // TODO
}
