#define DEF__REGULARMESSAGE_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <RegularMessage.h>
#endif

#include <RegularMessage.h>
#include <AbstractMessage.h>

using namespace std;

RegularMessage::RegularMessage(const int recipientId, const int senderId, const int messageId, const int timestamp, const string message) : AbstractMessage(recipientId, senderId, messageId, timestamp) {
  this->message = message;  
}

int RegularMessage::sendPacket() {
    // TODO
}
