#define DEF__RECEIVEDACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <ReceivedACK.h>
#endif

#include <ReceivedACK.h>
#include <AbstractACK.h>

using namespace std;

ReceivedACK::ReceivedACK(const int recipientId, const int senderId, const int messageId, const int timestamp) : AbstractACK(recipientId, senderId, messageId, timestamp) {}

int ReceivedACK::sendPacket() {
    // TODO
}
