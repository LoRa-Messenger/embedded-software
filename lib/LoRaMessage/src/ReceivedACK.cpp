#define DEF__RECEIVEDACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <ReceivedACK.h>
#endif

#include <ReceivedACK.h>
#include <AbstractACK.h>

using namespace std;

ReceivedACK::ReceivedACK(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp) : AbstractACK(recipientId, senderId, messageId, timestamp) {}

int ReceivedACK::sendPacket() {
    // TODO
}
